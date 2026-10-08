// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Extension/UIExtensionPointWidget.h"

// Hodge 自定义 LocalPlayer。
// 用于获取当前本地玩家，并监听该 LocalPlayer 对应 PlayerState 的设置。
#include "Core/LocalPlayer/HodgeLocalPlayerBase.h"

// Slate Overlay。
// 编辑器设计时用于显示 ExtensionPoint 的占位提示。
#include "Widgets/SOverlay.h"

// Slate 文本控件。
// 编辑器设计时用于显示当前 ExtensionPointTag。
#include "Widgets/Text/STextBlock.h"

// Widget 蓝图编译日志。
// 编辑器下用于检查 ExtensionPointTag 等配置是否合法。
#include "Editor/WidgetCompilerLog.h"

// UObject Token。
// 编译错误信息中可以附带具体 UObject，方便编辑器定位问题对象。
#include "Misc/UObjectToken.h"

// PlayerState。
// 当前 ExtensionPointWidget 会额外以 PlayerState 作为 Context 注册 ExtensionPoint。
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIExtensionPointWidget)

// 当前文件使用的本地化文本命名空间。
#define LOCTEXT_NAMESPACE "UIExtension"

/////////////////////////////////////////////////////
// UUIExtensionPointWidget

// 构造函数。
// 当前没有额外初始化逻辑，直接使用父类 UDynamicEntryBoxBase 的默认行为。
UUIExtensionPointWidget::UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUIExtensionPointWidget::SetExtensionPointTag(FGameplayTag Tag)
{
    if (!GetCachedWidget().IsValid()) { ExtensionPointTag = Tag; }
}

// 当前 UWidget 释放底层 Slate 资源时调用。
void UUIExtensionPointWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	// 在 Widget 被释放之前，
	// 先注销所有 ExtensionPoint，并清理由 Extension 创建出来的动态 Entry。
	ResetExtensionPoint();

	// 最后执行父类的 Slate 资源释放逻辑。
	Super::ReleaseSlateResources(bReleaseChildren);
}

// 重建当前 UMG Widget 对应的底层 Slate Widget。
TSharedRef<SWidget> UUIExtensionPointWidget::RebuildWidget()
{
	// 运行时并且配置了有效 ExtensionPointTag 时，
	// 正式将当前 Widget 注册为一个 UI ExtensionPoint。
	if (!IsDesignTime() && ExtensionPointTag.IsValid())
	{
		// RebuildWidget 可能被重复调用，
		// 因此先清理之前已经存在的注册和动态 Entry。
		ResetExtensionPoint();

		// 注册基础 ExtensionPoint：
		// 包括无 Context 的 ExtensionPoint 和 LocalPlayer Context 的 ExtensionPoint。
		RegisterExtensionPoint();

		// 监听当前 LocalPlayer 的 PlayerState 设置事件。
		//
		// CallAndRegister 表示：
		// 如果 PlayerState 当前已经存在，则可以立即触发；
		// 同时注册后续 PlayerState 设置时的回调。
		BoundLocalPlayer = GetOwningLocalPlayer<UHodgeLocalPlayerBase>();
		if (BoundLocalPlayer.IsValid())
		{
			PlayerStateDelegate = BoundLocalPlayer->CallAndRegister_OnPlayerStateSet(
				UHodgeLocalPlayerBase::FPlayerStateSetDelegate::FDelegate::CreateUObject(
					this, &UUIExtensionPointWidget::RegisterExtensionPointForPlayerState)
			);
		}
	}

	// 编辑器设计时不真正注册 ExtensionPoint，
	// 而是构造一个简单的 Slate 占位 UI，
	// 方便 UI 设计人员在 Widget Blueprint 中看到这个扩展点的位置和 Tag。
	if (IsDesignTime())
	{
		// 动态生成编辑器中显示的 ExtensionPoint 提示文本。
		auto GetExtensionPointText = [this]()
		{
			return FText::Format(
				LOCTEXT("DesignTime_ExtensionPointLabel", "Extension Point\n{0}"),
				FText::FromName(ExtensionPointTag.GetTagName()));
		};

		// 创建一个简单的 Slate Overlay 作为设计时占位控件。
		TSharedRef<SOverlay> MessageBox = SNew(SOverlay);

		// 向 Overlay 中添加一个居中的文本控件。
		MessageBox->AddSlot()
		          .Padding(5.0f)
		          .HAlign(HAlign_Center)
		          .VAlign(VAlign_Center)
		[
			// 显示：
			// Extension Point
			// UI.xxx.xxx
			SNew(STextBlock)
			.Justification(ETextJustify::Center)
			.Text_Lambda(GetExtensionPointText)
		];

		// 设计时直接返回这个占位 Slate Widget。
		return MessageBox;
	}
	else
	{
		// 运行时使用 UDynamicEntryBoxBase 正常构建出来的底层 Widget，
		// 后续动态创建的 Entry 会被添加到这个容器中。
		return Super::RebuildWidget();
	}
}

// 重置当前 ExtensionPointWidget 的运行时状态。
void UUIExtensionPointWidget::ResetExtensionPoint()
{
	if (BoundLocalPlayer.IsValid()) { BoundLocalPlayer->OnPlayerStateSet.Remove(PlayerStateDelegate); }
	BoundLocalPlayer.Reset();
	PlayerStateDelegate.Reset();
	PlayerStatePoint.Unregister();

	// 清空 UDynamicEntryBoxBase 当前已经创建的所有动态 Entry。
	ResetInternal();

	// 清空 ExtensionHandle → Widget 实例之间的映射。
	ExtensionMapping.Reset();

	// 注销当前 Widget 已经注册到 UUIExtensionSubsystem 的所有 ExtensionPoint。
	for (FUIExtensionPointHandle& Handle : ExtensionPointHandles)
	{
		Handle.Unregister();
	}

	// 清空本地保存的 ExtensionPoint Handle。
	ExtensionPointHandles.Reset();
}

// 注册当前 Widget 对应的基础 ExtensionPoint。
void UUIExtensionPointWidget::RegisterExtensionPoint()
{
	// 从当前 World 获取 UIExtensionSubsystem。
	// UI Extension 的所有注册和匹配都统一由这个 WorldSubsystem 管理。
	if (UUIExtensionSubsystem* ExtensionSubsystem = GetWorld()->GetSubsystem<UUIExtensionSubsystem>())
	{
		// 构造当前 ExtensionPoint 允许接收的数据类型集合。
		TArray<UClass*> AllowedDataClasses;

		// 默认始终允许接收 UUserWidget 及其子类。
		//
		// 这样其他系统可以直接：
		// RegisterExtensionAsWidget(...)
		// 将一个 WidgetClass 注册到当前 ExtensionPoint。
		AllowedDataClasses.Add(UUserWidget::StaticClass());

		// 再追加当前 ExtensionPointWidget 配置的业务数据类型。
		//
		// 这样 Extension 除了直接提供 WidgetClass，
		// 还可以提供 QuestData、TeamData 等普通 UObject 数据。
		AllowedDataClasses.Append(DataClasses);

		// 注册第一种 ExtensionPoint：
		// 不带 ContextObject 的全局 ExtensionPoint。
		//
		// 只有同样没有 ContextObject 的 Extension 才能通过 Context 契约。
		ExtensionPointHandles.Add(ExtensionSubsystem->RegisterExtensionPoint(
			ExtensionPointTag, ExtensionPointTagMatch, AllowedDataClasses,
			FExtendExtensionPointDelegate::CreateUObject(this, &ThisClass::OnAddOrRemoveExtension)
		));

		// 注册第二种 ExtensionPoint：
		// 使用当前 OwningLocalPlayer 作为 ContextObject。
		//
		// 这样其他系统可以把 Extension 精确注册给某个 LocalPlayer，
		// 避免多个本地玩家之间的 UI 内容串到一起。
		ExtensionPointHandles.Add(ExtensionSubsystem->RegisterExtensionPointForContext(
			ExtensionPointTag, GetOwningLocalPlayer(), ExtensionPointTagMatch, AllowedDataClasses,
			FExtendExtensionPointDelegate::CreateUObject(this, &ThisClass::OnAddOrRemoveExtension)
		));
	}
}

// 为当前玩家的 PlayerState 再注册一个带 PlayerState Context 的 ExtensionPoint。
void UUIExtensionPointWidget::RegisterExtensionPointForPlayerState(UHodgeLocalPlayerBase* LocalPlayer,
                                                                   APlayerState* PlayerState)
{
	PlayerStatePoint.Unregister();
	if (!PlayerState) { return; }
	// 从当前 World 获取 UIExtensionSubsystem。
	if (UUIExtensionSubsystem* ExtensionSubsystem = GetWorld()->GetSubsystem<UUIExtensionSubsystem>())
	{
		// 与基础 ExtensionPoint 使用相同的数据类型契约。
		TArray<UClass*> AllowedDataClasses;

		// 默认允许直接注册 UUserWidget Class。
		AllowedDataClasses.Add(UUserWidget::StaticClass());

		// 同时允许当前 Widget 配置的额外业务 Data 类型。
		AllowedDataClasses.Append(DataClasses);

		// 注册第三种 ExtensionPoint：
		// 使用当前 PlayerState 作为 ContextObject。
		//
		// 这样 Gameplay / GameFeature 可以把某条 UI Extension
		// 精确关联到指定玩家的 PlayerState。
		PlayerStatePoint = ExtensionSubsystem->RegisterExtensionPointForContext(
			ExtensionPointTag, PlayerState, ExtensionPointTagMatch, AllowedDataClasses,
			FExtendExtensionPointDelegate::CreateUObject(this, &ThisClass::OnAddOrRemoveExtension)
		);
	}
}

// 当前 ExtensionPoint 收到 Extension Added / Removed 通知时执行。
void UUIExtensionPointWidget::OnAddOrRemoveExtension(EUIExtensionAction Action, const FUIExtensionRequest& Request)
{
	// 有新的 Extension 匹配到当前 ExtensionPoint。
	if (Action == EUIExtensionAction::Added)
	{
		// 取出 Extension 携带的实际数据。
		UObject* Data = Request.Data;

		// 第一种情况：
		// 尝试直接把 Data 当成 UUserWidget Class。
		//
		// RegisterExtensionAsWidget(...) 注册进来的 WidgetClass
		// 就会走这条路径。
		TSubclassOf<UUserWidget> WidgetClass(Cast<UClass>(Data));

		// Data 本身就是有效的 UUserWidget Class。
		if (WidgetClass)
		{
			// 通过 UDynamicEntryBoxBase 创建并加入一个新的动态 Entry Widget。
			UUserWidget* Widget = CreateEntryInternal(WidgetClass);

			// 建立：
			// ExtensionHandle → Widget实例
			// 的映射。
			//
			// 后续收到 Removed 时就能精确找到应该删除哪个 Widget。
			ExtensionMapping.Add(Request.ExtensionHandle, Widget);
		}

		// 第二种情况：
		// Data 不是 WidgetClass，
		// 但当前 ExtensionPoint 配置了额外允许接收的业务 Data 类型。
		else if (DataClasses.Num() > 0)
		{
			// 必须绑定 GetWidgetClassForData，
			// 才知道这种业务 Data 应该使用什么 Widget 进行展示。
			if (GetWidgetClassForData.IsBound())
			{
				// 将业务 Data 转换为对应的 Widget Class。
				//
				// 例如：
				// QuestData → WBP_QuestEntry
				WidgetClass = GetWidgetClassForData.Execute(Data);

				// If the data is irrelevant they can just return no widget class.
				// 如果调用方认为这条 Data 不需要显示，
				// 可以直接返回空 WidgetClass，从而忽略这条 Extension。
				if (WidgetClass)
				{
					// 根据得到的 WidgetClass 创建动态 Entry。
					if (UUserWidget* Widget = CreateEntryInternal(WidgetClass))
					{
						// 保存 ExtensionHandle → Widget 实例映射，
						// 用于之后精确移除。
						ExtensionMapping.Add(Request.ExtensionHandle, Widget);

						// 将原始业务 Data 交给刚创建的 Widget，
						// 让外部蓝图 / 逻辑完成具体的数据绑定和初始化。
						ConfigureWidgetForData.ExecuteIfBound(Widget, Data);
					}
				}
			}
		}
	}

	// 非 Added 的情况按照 Extension 被移除处理。
	else
	{
		// 根据 ExtensionHandle 找到当初为这条 Extension 创建的 Widget。
		if (UUserWidget* Extension = ExtensionMapping.FindRef(Request.ExtensionHandle))
		{
			// 从 UDynamicEntryBoxBase 中移除对应 Entry Widget。
			RemoveEntryInternal(Extension);

			// 删除 ExtensionHandle → Widget 的映射记录。
			ExtensionMapping.Remove(Request.ExtensionHandle);
		}
	}
}

#if WITH_EDITOR

// Widget Blueprint 编译时检查当前 ExtensionPointWidget 的默认配置。
void UUIExtensionPointWidget::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	// 先执行父类的默认配置检查。
	Super::ValidateCompiledDefaults(CompileLog);

	// We don't care if the CDO doesn't have a specific tag.
	// CDO（Class Default Object）本身不要求配置具体 ExtensionPointTag，
	// 这里只检查真正放进 Widget Blueprint 中的实例。
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// 每个实际使用的 ExtensionPointWidget 都必须配置有效的 GameplayTag，
		// 否则其他 Extension 无法通过 Tag 找到这个插槽。
		if (!ExtensionPointTag.IsValid())
		{
			// 向 Widget 编译器报告错误。
			TSharedRef<FTokenizedMessage> Message = CompileLog.Error(FText::Format(
				LOCTEXT("UUIExtensionPointWidget_NoTag",
				        "{0} has no ExtensionPointTag specified - All extension points must specify a tag so they can be located."),
				FText::FromString(GetName())));

			// 将当前 Widget 对象附加到错误信息，
			// 方便编辑器直接定位到发生问题的对象。
			Message->AddToken(FUObjectToken::Create(this));
		}
	}
}
#endif

/////////////////////////////////////////////////////

// 结束当前文件的本地化文本命名空间。
#undef LOCTEXT_NAMESPACE
