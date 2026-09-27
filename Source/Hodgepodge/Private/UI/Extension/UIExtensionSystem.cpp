// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Extension/UIExtensionSystem.h"

// UUserWidget 定义。
// Extension 可以直接把某个 UserWidget Class 作为 Data 注册进系统。
#include "Blueprint/UserWidget.h"

// FFrame 定义。
// 蓝图包装函数参数非法时，通过 FFrame::KismetExecutionMessage 输出蓝图运行时错误。
#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIExtensionSystem)

// 定义 UIExtension 系统日志分类。
DEFINE_LOG_CATEGORY(LogUIExtension);

// Subsystem 初始化集合前向声明。
class FSubsystemCollectionBase;

//=========================================================

// 注销当前 Handle 对应的 ExtensionPoint。
void FUIExtensionPointHandle::Unregister()
{
	// ExtensionSource 是弱引用，
	// 只有创建当前 Handle 的 UIExtensionSubsystem 仍然有效时才能进行注销。
	if (UUIExtensionSubsystem* ExtensionSourcePtr = ExtensionSource.Get())
	{
		// 将当前 Handle 交还给注册它的 Subsystem，由 Subsystem 完成真正的移除。
		ExtensionSourcePtr->UnregisterExtensionPoint(*this);
	}
}

//=========================================================

// 注销当前 Handle 对应的 Extension。
void FUIExtensionHandle::Unregister()
{
	// 获取当初创建当前 Extension 的 UIExtensionSubsystem。
	if (UUIExtensionSubsystem* ExtensionSourcePtr = ExtensionSource.Get())
	{
		// 通过 Subsystem 从 ExtensionMap 中注销当前 Extension，
		// 同时向匹配的 ExtensionPoint 广播 Removed。
		ExtensionSourcePtr->UnregisterExtension(*this);
	}
}

//=========================================================

// 检查一个 Extension 是否满足当前 ExtensionPoint 声明的契约。
bool FUIExtensionPoint::DoesExtensionPassContract(const FUIExtension* Extension) const
{
	// Extension 必须携带有效的 Data。
	if (UObject* DataPtr = Extension->Data)
	{
		// Context 的匹配规则：
		// 1. ExtensionPoint 和 Extension 都没有指定 Context；
		// 2. 或者两边指定的是同一个 ContextObject。
		const bool bMatchesContext =
			(ContextObject.IsExplicitlyNull() && Extension->ContextObject.IsExplicitlyNull()) ||
			ContextObject == Extension->ContextObject;

		// Make sure the contexts match.
		// Context 匹配后才继续检查 Data 类型。
		if (bMatchesContext)
		{
			// The data can either be the literal class of the data type, or a instance of the class type.

			// Extension 的 Data 有两种常见形式：
			//
			// 1. Data 本身就是 UClass，例如注册一个 WBP_xxx 的 WidgetClass；
			// 2. Data 是某个 UObject 实例，例如某个数据对象。
			//
			// 如果 Data 本身是 UClass，就直接把它当成待检查的类型；
			// 否则获取这个 UObject 实例自身的 Class。
			const UClass* DataClass = DataPtr->IsA(UClass::StaticClass()) ? Cast<UClass>(DataPtr) : DataPtr->GetClass();

			// 遍历当前 ExtensionPoint 声明允许接收的所有数据类型。
			for (const UClass* AllowedDataClass : AllowedDataClasses)
			{
				// DataClass 只要：
				// 1. 是 AllowedDataClass 本身或其子类；
				// 2. 或实现了 AllowedDataClass 所代表的接口；
				// 就认为满足当前 ExtensionPoint 的数据类型契约。
				if (DataClass->IsChildOf(AllowedDataClass) || DataClass->ImplementsInterface(AllowedDataClass))
				{
					return true;
				}
			}
		}
	}

	// Data 无效、Context 不匹配或 Data 类型不符合要求时，都认为契约不通过。
	return false;
}

//=========================================================

// 手动向 UE GC 报告 UIExtensionSubsystem 内部普通 C++ 结构所持有的 UObject 引用。
void UUIExtensionSubsystem::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	// 先执行父类引用收集逻辑。
	Super::AddReferencedObjects(InThis, Collector);

	// 确保当前 UObject 是 UUIExtensionSubsystem。
	if (UUIExtensionSubsystem* ExtensionSubsystem = Cast<UUIExtensionSubsystem>(InThis))
	{
		// 遍历当前 World 中所有 ExtensionPoint。
		for (auto MapIt = ExtensionSubsystem->ExtensionPointMap.CreateIterator(); MapIt; ++MapIt)
		{
			// 同一个 GameplayTag 下可以存在多个 ExtensionPoint。
			for (const TSharedPtr<FUIExtensionPoint>& ValueElement : MapIt.Value())
			{
				// FUIExtensionPoint 是普通 C++ 结构，不是 UObject，
				// 因此主动告诉 GC：AllowedDataClasses 中的 UClass 仍然正在被使用。
				Collector.AddReferencedObjects(ValueElement->AllowedDataClasses);
			}
		}

		// 遍历当前 World 中所有 Extension。
		for (auto MapIt = ExtensionSubsystem->ExtensionMap.CreateIterator(); MapIt; ++MapIt)
		{
			// 同一个 GameplayTag 下可以注册多个 Extension。
			for (const TSharedPtr<FUIExtension>& ValueElement : MapIt.Value())
			{
				// FUIExtension 同样不是 UObject，
				// 主动告诉 GC：Extension 携带的 Data 仍然正在被系统使用。
				Collector.AddReferencedObject(ValueElement->Data);
			}
		}
	}
}

// WorldSubsystem 初始化入口。
void UUIExtensionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// 当前没有额外初始化逻辑，只执行父类初始化。
	Super::Initialize(Collection);
}

// WorldSubsystem 销毁入口。
void UUIExtensionSubsystem::Deinitialize()
{
	// 当前没有额外销毁逻辑，只执行父类清理。
	Super::Deinitialize();
}

// 注册一个不指定 ContextObject 的 ExtensionPoint。
FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag,
                                                                      EUIExtensionPointMatch ExtensionPointTagMatchType,
                                                                      const TArray<UClass*>& AllowedDataClasses,
                                                                      FExtendExtensionPointDelegate ExtensionCallback)
{
	// 统一转交给带 Context 的完整版本，
	// nullptr 表示这个 ExtensionPoint 不绑定具体 ContextObject。
	return RegisterExtensionPointForContext(ExtensionPointTag, nullptr, ExtensionPointTagMatchType, AllowedDataClasses,
	                                        ExtensionCallback);
}

// 注册一个带可选 ContextObject 的 ExtensionPoint。
FUIExtensionPointHandle UUIExtensionSubsystem::RegisterExtensionPointForContext(
	const FGameplayTag& ExtensionPointTag, UObject* ContextObject, EUIExtensionPointMatch ExtensionPointTagMatchType,
	const TArray<UClass*>& AllowedDataClasses, FExtendExtensionPointDelegate ExtensionCallback)
{
	// ExtensionPoint 必须拥有有效的 GameplayTag。
	if (!ExtensionPointTag.IsValid())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	// ExtensionPoint 必须提供有效回调，
	// 否则即使匹配到 Extension 也没有接收通知的地方。
	if (!ExtensionCallback.IsBound())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	// ExtensionPoint 必须至少声明一种允许接收的数据类型。
	if (AllowedDataClasses.Num() == 0)
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension point."));
		return FUIExtensionPointHandle();
	}

	// 根据 GameplayTag 找到对应 ExtensionPoint 列表；
	// 如果不存在则自动创建。
	FExtensionPointList& List = ExtensionPointMap.FindOrAdd(ExtensionPointTag);

	// 创建新的 FUIExtensionPoint，
	// 并加入当前 Tag 对应的 ExtensionPoint 列表。
	TSharedPtr<FUIExtensionPoint>& Entry = List.Add_GetRef(MakeShared<FUIExtensionPoint>());

	// 保存 ExtensionPoint 所监听的 GameplayTag。
	Entry->ExtensionPointTag = ExtensionPointTag;

	// 保存可选上下文，用于玩家 / 对象级隔离。
	Entry->ContextObject = ContextObject;

	// 保存 ExactMatch / PartialMatch 匹配方式。
	Entry->ExtensionPointTagMatchType = ExtensionPointTagMatchType;

	// 保存当前 ExtensionPoint 能接受的数据类型契约。
	Entry->AllowedDataClasses = AllowedDataClasses;

	// 将回调所有权移动到 ExtensionPoint 中保存。
	Entry->Callback = MoveTemp(ExtensionCallback);

	UE_LOG(LogUIExtension, Verbose, TEXT("Extension Point [%s] Registered"), *ExtensionPointTag.ToString());

	// ExtensionPoint 可能比 Extension 注册得更晚。
	//
	// 因此新 ExtensionPoint 注册完成后，
	// 立即检查系统中已经存在的 Extension，
	// 把符合条件的已有 Extension 以 Added 形式补通知给它。
	NotifyExtensionPointOfExtensions(Entry);

	// 返回 Handle，供调用方以后注销这个 ExtensionPoint。
	return FUIExtensionPointHandle(this, Entry);
}

// 将 WidgetClass 注册成一个不带 ContextObject 的 Extension。
FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag,
                                                                    TSubclassOf<UUserWidget> WidgetClass,
                                                                    int32 Priority)
{
	// WidgetClass 本质上作为 UObject Data 存入 Extension，
	// 所以统一走 RegisterExtensionAsData。
	return RegisterExtensionAsData(ExtensionPointTag, nullptr, WidgetClass, Priority);
}

// 将 WidgetClass 注册成一个带 ContextObject 的 Extension。
FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext(
	const FGameplayTag& ExtensionPointTag, UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	// WidgetClass 仍然作为 Data 保存，
	// 额外携带 ContextObject 用于上下文匹配。
	return RegisterExtensionAsData(ExtensionPointTag, ContextObject, WidgetClass, Priority);
}

// 注册一条真正的 Extension 数据。
FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag,
                                                                  UObject* ContextObject, UObject* Data, int32 Priority)
{
	// Extension 必须注册到一个有效 GameplayTag。
	if (!ExtensionPointTag.IsValid())
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension."));
		return FUIExtensionHandle();
	}

	// Extension 必须携带有效 Data。
	if (!Data)
	{
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to register an invalid extension."));
		return FUIExtensionHandle();
	}

	// 找到该 GameplayTag 对应的 Extension 列表；
	// 如果不存在则创建。
	FExtensionList& List = ExtensionMap.FindOrAdd(ExtensionPointTag);

	// 创建新的 Extension 并加入列表。
	TSharedPtr<FUIExtension>& Entry = List.Add_GetRef(MakeShared<FUIExtension>());

	// 保存目标 ExtensionPoint Tag。
	Entry->ExtensionPointTag = ExtensionPointTag;

	// 保存可选上下文。
	Entry->ContextObject = ContextObject;

	// 保存实际 WidgetClass / UObject Data。
	Entry->Data = Data;

	// 保存 UI 排序优先级。
	Entry->Priority = Priority;

	// 根据是否存在 ContextObject 输出对应注册日志。
	// 注意：上游此处的判定与两个分支的内容相反（有 Context 却走短格式），
	// 这里按镜像函数 UnregisterExtension 的极性修正，属对上游的有意偏离。
	if (ContextObject == nullptr)
	{
		UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] @ [%s] Registered"), *GetNameSafe(Data),
		       *ExtensionPointTag.ToString());
	}
	else
	{
		UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] for [%s] @ [%s] Registered"), *GetNameSafe(Data),
		       *GetNameSafe(ContextObject), *ExtensionPointTag.ToString());
	}

	// Extension 可能比 ExtensionPoint 注册得更晚。
	//
	// 因此新 Extension 注册完成后，
	// 立即查找所有已经存在并且与它匹配的 ExtensionPoint，
	// 向这些 ExtensionPoint 广播 Added。
	NotifyExtensionPointsOfExtension(EUIExtensionAction::Added, Entry);

	// 返回 Handle，供调用方以后精确注销当前 Extension。
	return FUIExtensionHandle(this, Entry);
}

// 当一个新的 ExtensionPoint 注册时，
// 主动寻找系统中已经存在并符合它要求的 Extension。
void UUIExtensionSubsystem::NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint)
{
	// 从 ExtensionPoint 自己的 Tag 开始不断向父 Tag 查找。
	//
	// 例如：
	// UI.HUD.Right.Skill
	//       ↓
	// UI.HUD.Right
	//       ↓
	// UI.HUD
	//       ↓
	// UI
	for (FGameplayTag Tag = ExtensionPoint->ExtensionPointTag; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		// 查找当前 Tag 下已经注册的 Extension。
		if (const FExtensionList* ListPtr = ExtensionMap.Find(Tag))
		{
			// Copy in case there are removals while handling callbacks
			// 复制一份列表。
			// 因为 Callback 执行期间可能反过来注销 Extension，
			// 避免正在遍历的原始数组被修改导致迭代失效。
			FExtensionList ExtensionArray(*ListPtr);

			// 遍历当前 Tag 下所有 Extension。
			for (const TSharedPtr<FUIExtension>& Extension : ExtensionArray)
			{
				// 继续检查 ContextObject 和 AllowedDataClasses 等契约。
				if (ExtensionPoint->DoesExtensionPassContract(Extension.Get()))
				{
					// 将内部 Extension 转换成统一对外请求结构。
					FUIExtensionRequest Request = CreateExtensionRequest(Extension);

					// 对 ExtensionPoint 补发 Added，
					// 告诉它：这个 Extension 在你注册之前就已经存在。
					ExtensionPoint->Callback.ExecuteIfBound(EUIExtensionAction::Added, Request);
				}
			}
		}

		// ExactMatch 只检查 ExtensionPoint 自己的 Tag，
		// 不继续向父 Tag 查找。
		if (ExtensionPoint->ExtensionPointTagMatchType == EUIExtensionPointMatch::ExactMatch)
		{
			break;
		}
	}
}

// 当某条 Extension 被 Added / Removed 时，
// 主动通知所有已经存在并且能够匹配它的 ExtensionPoint。
void UUIExtensionSubsystem::NotifyExtensionPointsOfExtension(EUIExtensionAction Action,
                                                             TSharedPtr<FUIExtension>& Extension)
{
	// 标记当前是不是 Extension 自己最初注册的 Tag。
	bool bOnInitialTag = true;

	// 从 Extension 自己的 Tag 开始不断向父 Tag 查找 ExtensionPoint。
	//
	// 例如 Extension 注册在：
	// UI.HUD.Right.Skill
	//
	// 则依次检查：
	// UI.HUD.Right.Skill
	// UI.HUD.Right
	// UI.HUD
	// UI
	for (FGameplayTag Tag = Extension->ExtensionPointTag; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		// 查找当前 Tag 下已经注册的 ExtensionPoint。
		if (const FExtensionPointList* ListPtr = ExtensionPointMap.Find(Tag))
		{
			// Copy in case there are removals while handling callbacks
			// 回调期间可能注销 ExtensionPoint，
			// 所以复制数组后再遍历，避免修改原始容器造成迭代失效。
			FExtensionPointList ExtensionPointArray(*ListPtr);

			// 检查当前 Tag 下所有 ExtensionPoint。
			for (const TSharedPtr<FUIExtensionPoint>& ExtensionPoint : ExtensionPointArray)
			{
				// 如果当前就是 Extension 自己的 Tag，则 Exact / Partial 都可以匹配。
				//
				// 如果已经向父 Tag 查找，则只有声明 PartialMatch 的 ExtensionPoint
				// 才允许接收来自子 Tag 的 Extension。
				if (bOnInitialTag || (ExtensionPoint->ExtensionPointTagMatchType ==
					EUIExtensionPointMatch::PartialMatch))
				{
					// 继续检查 ContextObject 和 AllowedDataClasses 契约。
					if (ExtensionPoint->DoesExtensionPassContract(Extension.Get()))
					{
						// 构造对外通知数据。
						FUIExtensionRequest Request = CreateExtensionRequest(Extension);

						// 将 Added / Removed 原样通知给 ExtensionPoint。
						ExtensionPoint->Callback.ExecuteIfBound(Action, Request);
					}
				}
			}
		}

		// 第一次循环结束后，后续检查的都是父 Tag。
		bOnInitialTag = false;
	}
}

// 注销一条已经注册的 Extension。
void UUIExtensionSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)
{
	// Handle 必须仍然有效。
	if (ExtensionHandle.IsValid())
	{
		// 防止拿其他 UUIExtensionSubsystem 创建的 Handle
		// 到当前 Subsystem 中进行注销。
		checkf(ExtensionHandle.ExtensionSource == this,
		       TEXT("Trying to unregister an extension that's not from this extension subsystem."));

		// 从 Handle 中取得真正的 Extension 数据。
		TSharedPtr<FUIExtension> Extension = ExtensionHandle.DataPtr;

		// 根据 Extension 注册时的 GameplayTag 找到对应列表。
		if (FExtensionList* ListPtr = ExtensionMap.Find(Extension->ExtensionPointTag))
		{
			// 输出不带 Context 的注销日志。
			if (Extension->ContextObject.IsExplicitlyNull())
			{
				UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] @ [%s] Unregistered"),
				       *GetNameSafe(Extension->Data), *Extension->ExtensionPointTag.ToString());
			}
			else
			{
				// 输出带 Context 的注销日志。
				UE_LOG(LogUIExtension, Verbose, TEXT("Extension [%s] for [%s] @ [%s] Unregistered"),
				       *GetNameSafe(Extension->Data), *GetNameSafe(Extension->ContextObject.Get()),
				       *Extension->ExtensionPointTag.ToString());
			}

			// 在真正从 ExtensionMap 删除之前，
			// 先通知所有匹配的 ExtensionPoint：这条 Extension 已经 Removed。
			NotifyExtensionPointsOfExtension(EUIExtensionAction::Removed, Extension);

			// 从当前 Tag 对应的 Extension 列表中移除。
			// RemoveSwap 不保证原数组顺序，但删除效率更高。
			ListPtr->RemoveSwap(Extension);

			// 当前 Tag 已经没有任何 Extension 时，
			// 直接把整个 Tag Entry 从 Map 中删除。
			if (ListPtr->Num() == 0)
			{
				ExtensionMap.Remove(Extension->ExtensionPointTag);
			}
		}
	}
	else
	{
		// 无效 Handle 无法注销。
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to unregister an invalid Handle."));
	}
}

// 注销一个已经注册的 ExtensionPoint。
void UUIExtensionSubsystem::UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle)
{
	// Handle 必须有效。
	if (ExtensionPointHandle.IsValid())
	{
		// 确保当前 Handle 确实由这个 Subsystem 创建。
		check(ExtensionPointHandle.ExtensionSource == this);

		// 获取 Handle 对应的真正 ExtensionPoint。
		const TSharedPtr<FUIExtensionPoint> ExtensionPoint = ExtensionPointHandle.DataPtr;

		// 找到该 GameplayTag 对应的 ExtensionPoint 列表。
		if (FExtensionPointList* ListPtr = ExtensionPointMap.Find(ExtensionPoint->ExtensionPointTag))
		{
			UE_LOG(LogUIExtension, Verbose, TEXT("Extension Point [%s] Unregistered"),
			       *ExtensionPoint->ExtensionPointTag.ToString());

			// 从列表中移除当前 ExtensionPoint。
			ListPtr->RemoveSwap(ExtensionPoint);

			// 如果这个 Tag 下已经不存在任何 ExtensionPoint，
			// 则将整个 Tag Entry 从 Map 删除。
			if (ListPtr->Num() == 0)
			{
				ExtensionPointMap.Remove(ExtensionPoint->ExtensionPointTag);
			}
		}
	}
	else
	{
		// 无效 Handle 无法注销。
		UE_LOG(LogUIExtension, Warning, TEXT("Trying to unregister an invalid Handle."));
	}
}

// 将内部 FUIExtension 转换为提供给 ExtensionPoint 的公开请求数据。
FUIExtensionRequest UUIExtensionSubsystem::CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension)
{
	FUIExtensionRequest Request;

	// 为当前 Extension 构造 Handle，
	// 让接收方能够唯一标识这条 Extension。
	Request.ExtensionHandle = FUIExtensionHandle(this, Extension);

	// 携带 Extension 注册的目标 Tag。
	Request.ExtensionPointTag = Extension->ExtensionPointTag;

	// 携带排序优先级。
	Request.Priority = Extension->Priority;

	// 携带真正的 WidgetClass / UObject Data。
	Request.Data = Extension->Data;

	// 将弱引用 ContextObject 转换为普通 UObject 指针交给请求接收方。
	Request.ContextObject = Extension->ContextObject.Get();

	return Request;
}

// RegisterExtensionPoint 的蓝图包装版本。
FUIExtensionPointHandle UUIExtensionSubsystem::K2_RegisterExtensionPoint(
	FGameplayTag ExtensionPointTag, EUIExtensionPointMatch ExtensionPointTagMatchType,
	const TArray<UClass*>& AllowedDataClasses, FExtendExtensionPointDynamicDelegate ExtensionCallback)
{
	// 底层 C++ 接口需要普通 Delegate，
	// 这里通过 WeakLambda 把蓝图 DynamicDelegate 转接到 C++ Delegate。
	return RegisterExtensionPoint(ExtensionPointTag, ExtensionPointTagMatchType, AllowedDataClasses,
	                              FExtendExtensionPointDelegate::CreateWeakLambda(
		                              ExtensionCallback.GetUObject(),
		                              [this, ExtensionCallback](EUIExtensionAction Action,
		                                                        const FUIExtensionRequest& Request)
		                              {
			                              // 将 C++ ExtensionPoint 通知继续转发给蓝图回调。
			                              ExtensionCallback.ExecuteIfBound(Action, Request);
		                              }));
}

// RegisterExtensionAsWidget 的蓝图包装版本。
FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag,
                                                                       TSubclassOf<UUserWidget> WidgetClass,
                                                                       int32 Priority)
{
	// 直接复用 C++ 注册逻辑。
	return RegisterExtensionAsWidget(ExtensionPointTag, WidgetClass, Priority);
}

// RegisterExtensionAsWidgetForContext 的蓝图包装版本。
FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsWidgetForContext(
	FGameplayTag ExtensionPointTag, TSubclassOf<UUserWidget> WidgetClass, UObject* ContextObject, int32 Priority)
{
	// 带 Context 的版本要求 ContextObject 必须有效。
	if (ContextObject)
	{
		return RegisterExtensionAsWidgetForContext(ExtensionPointTag, ContextObject, WidgetClass, Priority);
	}
	else
	{
		// 蓝图传入空 Context 时直接向 Kismet 输出运行时错误，
		// 并返回无效 Handle。
		FFrame::KismetExecutionMessage(
			TEXT("A null ContextObject was passed to Register Extension (Widget For Context)"), ELogVerbosity::Error);
		return FUIExtensionHandle();
	}
}

// RegisterExtensionAsData 的蓝图包装版本。
FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data,
                                                                     int32 Priority)
{
	// 不带 Context，因此传入 nullptr。
	return RegisterExtensionAsData(ExtensionPointTag, nullptr, Data, Priority);
}

// RegisterExtensionAsDataForContext 的蓝图包装版本。
FUIExtensionHandle UUIExtensionSubsystem::K2_RegisterExtensionAsDataForContext(
	FGameplayTag ExtensionPointTag, UObject* ContextObject, UObject* Data, int32 Priority)
{
	// 带 Context 的版本要求 ContextObject 必须有效。
	if (ContextObject)
	{
		return RegisterExtensionAsData(ExtensionPointTag, ContextObject, Data, Priority);
	}
	else
	{
		// 蓝图错误调用时输出明确的 Kismet 运行时错误。
		FFrame::KismetExecutionMessage(
			TEXT("A null ContextObject was passed to Register Extension (Data For Context)"), ELogVerbosity::Error);
		return FUIExtensionHandle();
	}
}

//=========================================================

// 蓝图辅助函数：通过 ExtensionHandle 注销 Extension。
void UUIExtensionHandleFunctions::Unregister(FUIExtensionHandle& Handle)
{
	Handle.Unregister();
}

// 蓝图辅助函数：检查 ExtensionHandle 是否有效。
bool UUIExtensionHandleFunctions::IsValid(FUIExtensionHandle& Handle)
{
	return Handle.IsValid();
}

//=========================================================

// 蓝图辅助函数：通过 ExtensionPointHandle 注销 ExtensionPoint。
void UUIExtensionPointHandleFunctions::Unregister(FUIExtensionPointHandle& Handle)
{
	Handle.Unregister();
}

// 蓝图辅助函数：检查 ExtensionPointHandle 是否有效。
bool UUIExtensionPointHandleFunctions::IsValid(FUIExtensionPointHandle& Handle)
{
	return Handle.IsValid();
}
