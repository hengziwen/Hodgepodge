// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// GameplayTag 容器。
// UIExtensionSystem 使用 GameplayTag 标识不同的 UI 扩展点。
#include "GameplayTagContainer.h"

// 蓝图函数库基类。
// 文件末尾的 HandleFunctions 用它向蓝图暴露 Handle 操作。
#include "Kismet/BlueprintFunctionLibrary.h"

// WorldSubsystem 基类。
// UUIExtensionSubsystem 的生命周期与当前 World 绑定。
#include "Subsystems/WorldSubsystem.h"

#include "UIExtensionSystem.generated.h"

// UI 扩展系统 Subsystem 前向声明。
class UUIExtensionSubsystem;

// UI 扩展通知时传递给 ExtensionPoint 的请求数据。
struct FUIExtensionRequest;

template <typename T>
class TSubclassOf;

// Subsystem 初始化集合。
class FSubsystemCollectionBase;

// UMG Widget 基类。
// Extension 可以直接注册一个 UserWidget Class 作为扩展内容。
class UUserWidget;

struct FFrame;

// UIExtension 系统日志分类。
DECLARE_LOG_CATEGORY_EXTERN(LogUIExtension, Log, All);

// Match rule for extension points
// ExtensionPoint 与 Extension 的 GameplayTag 匹配规则。
UENUM(BlueprintType)
enum class EUIExtensionPointMatch : uint8
{
	// An exact match will only receive extensions with exactly the same point
	// (e.g., registering for "A.B" will match a broadcast of A.B but not A.B.C)

	// 精确匹配：
	// ExtensionPoint 注册为 A.B 时，只接受同样注册在 A.B 上的 Extension，
	// 不接受 A.B.C 等子 Tag。
	ExactMatch,

	// A partial match will receive any extensions rooted in the same point
	// (e.g., registering for "A.B" will match a broadcast of A.B as well as A.B.C)

	// 部分匹配：
	// ExtensionPoint 注册为 A.B 时，
	// 除了 A.B 自身，还可以接受 A.B.C 等以 A.B 为父级的 Extension。
	PartialMatch
};

// Match rule for extension points
// ExtensionPoint 收到通知时，本次 Extension 发生的操作类型。
UENUM(BlueprintType)
enum class EUIExtensionAction : uint8
{
	// 有新的 Extension 加入。
	Added,

	// 已有 Extension 被移除。
	Removed
};

// C++ ExtensionPoint 回调。
// 当匹配的 Extension 被添加 / 移除时，
// 将操作类型和 ExtensionRequest 一起通知给 ExtensionPoint。
DECLARE_DELEGATE_TwoParams(FExtendExtensionPointDelegate, EUIExtensionAction Action,
                           const FUIExtensionRequest& Request);

/*
 *
 */
// 一条已经注册到 UIExtensionSystem 中的“UI 扩展内容”。
//
// 可以理解成：
// “我希望把 Data 这份内容注册到 ExtensionPointTag 指定的 UI 插槽中。”
//
// TSharedFromThis 允许该普通 C++ 结构通过 SharedPtr 管理生命周期。
struct FUIExtension : TSharedFromThis<FUIExtension>
{
public:
	/** The extension point this extension is intended for. */

	// 当前 Extension 希望进入哪个 UI ExtensionPoint。
	// 例如 UI.HUD.Right / UI.HUD.Ability 等。
	FGameplayTag ExtensionPointTag;

	// 当前 Extension 的优先级。
	// ExtensionPoint 可以利用它决定多个扩展内容之间的顺序。
	int32 Priority = INDEX_NONE;

	// 可选的上下文对象。
	// 用于进一步限定这条 Extension 属于哪个具体上下文，
	// 例如某个 Player、PlayerController 或其他业务对象。
	TWeakObjectPtr<UObject> ContextObject;

	//Kept alive by UUIExtensionSubsystem::AddReferencedObjects
	// 当前 Extension 携带的实际数据。
	//
	// Data 可以是 WidgetClass，也可以是其他 UObject 数据对象，
	// 最终由匹配到的 ExtensionPoint 决定如何使用。
	//
	// 该对象由 UUIExtensionSubsystem::AddReferencedObjects
	// 主动向 GC 报告引用以保证生命周期。
	TObjectPtr<UObject> Data = nullptr;
};

/**
 * 
 */
// 一条已经注册到 UIExtensionSystem 中的“UI 扩展点”。
//
// ExtensionPoint 可以理解成 UI 中预留的动态插槽：
// “凡是满足我的 Tag、Context、DataClass 契约的 Extension，都通知我。”
struct FUIExtensionPoint : TSharedFromThis<FUIExtensionPoint>
{
public:
	// 当前 UI 插槽监听的 GameplayTag。
	FGameplayTag ExtensionPointTag;

	// 当前 ExtensionPoint 所属的可选上下文对象。
	// 用于让同一个 Tag 的扩展点进一步按玩家 / 对象等上下文进行隔离。
	TWeakObjectPtr<UObject> ContextObject;

	// GameplayTag 使用精确匹配还是部分匹配。
	EUIExtensionPointMatch ExtensionPointTagMatchType = EUIExtensionPointMatch::ExactMatch;

	// 当前 ExtensionPoint 允许接收的数据类型集合。
	// Extension 携带的 Data 必须满足这里声明的数据类型契约。
	TArray<TObjectPtr<UClass>> AllowedDataClasses;

	// 当匹配的 Extension Added / Removed 时调用的回调。
	FExtendExtensionPointDelegate Callback;

	// Tests if the extension and the extension point match up, if they do then this extension point should learn
	// about this extension.

	// 检查指定 Extension 是否满足当前 ExtensionPoint 的契约。
	//
	// 通常会综合判断：
	// - GameplayTag 是否匹配
	// - ContextObject 是否匹配
	// - Data 类型是否属于 AllowedDataClasses
	//
	// 通过契约后，当前 ExtensionPoint 才应该收到该 Extension 的通知。
	bool DoesExtensionPassContract(const FUIExtension* Extension) const;
};

/**
 * 
 */
// ExtensionPoint 的外部操作句柄。
//
// 注册 ExtensionPoint 后返回该 Handle，
// 调用方以后不需要直接保存内部 FUIExtensionPoint，
// 只需要保存 Handle 就可以注销和判断有效性。
USTRUCT(BlueprintType)
struct HODGEPODGE_API FUIExtensionPointHandle
{
	GENERATED_BODY()

public:
	// 默认构造一个无效 Handle。
	FUIExtensionPointHandle()
	{
	}

	// 注销当前 Handle 对应的 ExtensionPoint。
	void Unregister();

	// 内部 ExtensionPoint 数据仍然存在时，Handle 有效。
	bool IsValid() const { return DataPtr.IsValid(); }

	// 两个 Handle 是否指向同一份内部 ExtensionPoint 数据。
	bool operator==(const FUIExtensionPointHandle& Other) const { return DataPtr == Other.DataPtr; }

	// 两个 Handle 是否指向不同的内部 ExtensionPoint 数据。
	bool operator!=(const FUIExtensionPointHandle& Other) const { return !operator==(Other); }

	// 使用内部 SharedPtr 指向对象的地址计算 Hash，
	// 允许 Handle 用于需要 Hash 的容器。
	friend uint32 GetTypeHash(const FUIExtensionPointHandle& Handle)
	{
		return PointerHash(Handle.DataPtr.Get());
	}

private:
	// 创建当前 ExtensionPoint 的 Subsystem。
	// Handle 注销时需要通过它找到真正的注册源。
	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;

	// 当前 Handle 对应的内部 ExtensionPoint 数据。
	TSharedPtr<FUIExtensionPoint> DataPtr;

	// 允许 UUIExtensionSubsystem 直接访问 Handle 私有成员。
	friend UUIExtensionSubsystem;

	// 仅供 UUIExtensionSubsystem 创建有效 Handle。
	FUIExtensionPointHandle(UUIExtensionSubsystem* InExtensionSource,
	                        const TSharedPtr<FUIExtensionPoint>& InDataPtr) : ExtensionSource(InExtensionSource),
	                                                                          DataPtr(InDataPtr)
	{
	}
};

// 为 FUIExtensionPointHandle 声明 Unreal Struct 操作特性。
template <>
struct TStructOpsTypeTraits<FUIExtensionPointHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionPointHandle>
{
	enum
	{
		// This ensures the opaque type is copied correctly in BPs
		// 允许这个内部包含 SharedPtr 的不透明 Handle 在蓝图中被正确复制。
		WithCopy = true,

		// 使用 operator== 判断两个 Handle 是否相同。
		WithIdenticalViaEquality = true,
	};
};

/**
 * 
 */
// Extension 的外部操作句柄。
//
// 注册 Extension 后返回该 Handle，
// 调用方以后通过它注销对应的动态 UI 内容。
USTRUCT(BlueprintType)
struct HODGEPODGE_API FUIExtensionHandle
{
	GENERATED_BODY()

public:
	// 默认构造一个无效 Handle。
	FUIExtensionHandle()
	{
	}

	// 注销当前 Handle 对应的 Extension。
	void Unregister();

	// 内部 Extension 数据存在时 Handle 有效。
	bool IsValid() const { return DataPtr.IsValid(); }

	// 两个 Handle 是否指向同一个 Extension。
	bool operator==(const FUIExtensionHandle& Other) const { return DataPtr == Other.DataPtr; }

	// 两个 Handle 是否指向不同 Extension。
	bool operator!=(const FUIExtensionHandle& Other) const { return !operator==(Other); }

	// 使用内部 Extension 对象地址生成 Hash。
	friend FORCEINLINE uint32 GetTypeHash(FUIExtensionHandle Handle)
	{
		return PointerHash(Handle.DataPtr.Get());
	}

private:
	// 创建当前 Extension 的 Subsystem。
	TWeakObjectPtr<UUIExtensionSubsystem> ExtensionSource;

	// 当前 Handle 对应的内部 Extension 数据。
	TSharedPtr<FUIExtension> DataPtr;

	// 允许 Subsystem 访问 Handle 私有成员。
	friend UUIExtensionSubsystem;

	// 仅供 UUIExtensionSubsystem 创建有效 Handle。
	FUIExtensionHandle(UUIExtensionSubsystem* InExtensionSource,
	                   const TSharedPtr<FUIExtension>& InDataPtr) : ExtensionSource(InExtensionSource),
	                                                                DataPtr(InDataPtr)
	{
	}
};

// 为 FUIExtensionHandle 声明 Unreal Struct 操作特性。
template <>
struct TStructOpsTypeTraits<FUIExtensionHandle> : public TStructOpsTypeTraitsBase2<FUIExtensionHandle>
{
	enum
	{
		// This ensures the opaque type is copied correctly in BPs
		// 允许内部包含 SharedPtr 的 Handle 在蓝图中被正确复制。
		WithCopy = true,

		// 使用 Handle 自己的 operator== 判断相等性。
		WithIdenticalViaEquality = true,
	};
};

/**
 * 
 */
// ExtensionPoint 收到 Added / Removed 通知时使用的统一请求数据。
//
// 它把内部 FUIExtension 中真正需要暴露给 UI 的信息整理出来，
// 使 ExtensionPoint 不需要直接操作内部 Extension 对象。
USTRUCT(BlueprintType)
struct FUIExtensionRequest
{
	GENERATED_BODY()

public:
	// 当前请求对应的 Extension Handle。
	// 接收方如果需要，可以利用它识别 / 操作具体 Extension。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FUIExtensionHandle ExtensionHandle;

	// 当前 Extension 注册到哪个 ExtensionPoint Tag。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ExtensionPointTag;

	// 当前 Extension 的优先级。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Priority = INDEX_NONE;

	// 当前 Extension 携带的实际数据。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> Data = nullptr;

	// 当前 Extension 对应的上下文对象。
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UObject> ContextObject = nullptr;
};

// 蓝图版本的 ExtensionPoint 回调。
// 当 Extension Added / Removed 时将 Action 和 Request 传给蓝图。
DECLARE_DYNAMIC_DELEGATE_TwoParams(FExtendExtensionPointDynamicDelegate, EUIExtensionAction, Action,
                                   const FUIExtensionRequest&, ExtensionRequest);

/**
 * 
 */
// UI 动态扩展系统的核心 WorldSubsystem。
//
// 它维护两类注册：
//
// 1. ExtensionPoint：
//    “UI 的什么位置愿意接收动态内容？”
//
// 2. Extension：
//    “哪个系统希望向某个位置动态提供什么内容？”
//
// Subsystem 根据 GameplayTag、ContextObject、AllowedDataClasses 等契约
// 将两边匹配起来，并通过 Added / Removed 回调通知 ExtensionPoint。
UCLASS()
class HODGEPODGE_API UUIExtensionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 注册一个不限定 ContextObject 的 ExtensionPoint。
	//
	// ExtensionPointTag：
	// 当前 UI 插槽监听的 Tag。
	//
	// ExtensionPointTagMatchType：
	// 使用 ExactMatch 还是 PartialMatch。
	//
	// AllowedDataClasses：
	// 当前插槽允许接受的数据类型。
	//
	// ExtensionCallback：
	// 匹配的 Extension Added / Removed 时触发的回调。
	FUIExtensionPointHandle RegisterExtensionPoint(const FGameplayTag& ExtensionPointTag,
	                                               EUIExtensionPointMatch ExtensionPointTagMatchType,
	                                               const TArray<UClass*>& AllowedDataClasses,
	                                               FExtendExtensionPointDelegate ExtensionCallback);

	// 注册一个带 ContextObject 限制的 ExtensionPoint。
	// 除了 Tag / DataClass 外，还要求 Extension 与指定上下文满足匹配关系。
	FUIExtensionPointHandle RegisterExtensionPointForContext(const FGameplayTag& ExtensionPointTag,
	                                                         UObject* ContextObject,
	                                                         EUIExtensionPointMatch ExtensionPointTagMatchType,
	                                                         const TArray<UClass*>& AllowedDataClasses,
	                                                         FExtendExtensionPointDelegate ExtensionCallback);

	// 将一个 UserWidget Class 作为 Extension 注册到指定 ExtensionPointTag。
	FUIExtensionHandle RegisterExtensionAsWidget(const FGameplayTag& ExtensionPointTag,
	                                             TSubclassOf<UUserWidget> WidgetClass, int32 Priority);

	// 将一个 UserWidget Class 作为指定 ContextObject 下的 Extension 注册。
	FUIExtensionHandle RegisterExtensionAsWidgetForContext(const FGameplayTag& ExtensionPointTag,
	                                                       UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass,
	                                                       int32 Priority);

	// 将任意 UObject Data 作为 Extension 注册。
	// 具体如何使用 Data，由匹配到的 ExtensionPoint 决定。
	FUIExtensionHandle RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag, UObject* ContextObject,
	                                           UObject* Data, int32 Priority);

	// 根据 Handle 注销已经注册的 Extension。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	void UnregisterExtension(const FUIExtensionHandle& ExtensionHandle);

	// 根据 Handle 注销已经注册的 ExtensionPoint。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	void UnregisterExtensionPoint(const FUIExtensionPointHandle& ExtensionPointHandle);

	// 向 UE GC 主动报告 Subsystem 内部 FUIExtension 等普通 C++ 结构持有的 UObject 引用。
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

protected:
	// WorldSubsystem 初始化。
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// WorldSubsystem 销毁 / World 结束时清理 Extension 与 ExtensionPoint。
	virtual void Deinitialize() override;

	// 新注册一个 ExtensionPoint 时，
	// 检查系统中已经存在的 Extension，
	// 将所有满足该 ExtensionPoint 契约的内容通知给它。
	void NotifyExtensionPointOfExtensions(TSharedPtr<FUIExtensionPoint>& ExtensionPoint);

	// 某个 Extension Added / Removed 时，
	// 检查所有可能匹配的 ExtensionPoint，
	// 并向满足契约的 ExtensionPoint 发送通知。
	void NotifyExtensionPointsOfExtension(EUIExtensionAction Action, TSharedPtr<FUIExtension>& Extension);

	// RegisterExtensionPoint 的蓝图包装版本。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
		meta = (DisplayName = "Register Extension Point"))
	FUIExtensionPointHandle K2_RegisterExtensionPoint(FGameplayTag ExtensionPointTag,
	                                                  EUIExtensionPointMatch ExtensionPointTagMatchType,
	                                                  const TArray<UClass*>& AllowedDataClasses,
	                                                  FExtendExtensionPointDynamicDelegate ExtensionCallback);

	// RegisterExtensionAsWidget 的蓝图包装版本。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension",
		meta = (DisplayName = "Register Extension (Widget)"))
	FUIExtensionHandle K2_RegisterExtensionAsWidget(FGameplayTag ExtensionPointTag,
	                                                TSubclassOf<UUserWidget> WidgetClass, int32 Priority = -1);

	/**
	 * Registers the widget (as data) for a specific player.  This means the extension points will receive a UIExtensionForPlayer data object
	 * that they can look at to determine if it's for whatever they consider their player.
	 */
	// 将 WidgetClass 作为带 ContextObject 的 Extension 注册。
	//
	// 典型用途是：
	// 同一个 ExtensionPointTag 可能同时存在于多个玩家 UI 中，
	// ContextObject 用来限定这份 Widget Extension 属于哪个玩家 / 上下文。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension",
		meta = (DisplayName = "Register Extension (Widget For Context)"))
	FUIExtensionHandle K2_RegisterExtensionAsWidgetForContext(FGameplayTag ExtensionPointTag,
	                                                          TSubclassOf<UUserWidget> WidgetClass,
	                                                          UObject* ContextObject, int32 Priority = -1);

	/**
	 * Registers the extension as data for any extension point that can make use of it.
	 */
	// 将任意 UObject 作为 Extension Data 注册。
	// 不指定 ContextObject。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
		meta = (DisplayName = "Register Extension (Data)"))
	FUIExtensionHandle K2_RegisterExtensionAsData(FGameplayTag ExtensionPointTag, UObject* Data, int32 Priority = -1);

	/**
	 * Registers the extension as data for any extension point that can make use of it.
	 */
	// 将任意 UObject 作为带 ContextObject 的 Extension Data 注册。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="UI Extension",
		meta = (DisplayName = "Register Extension (Data For Context)"))
	FUIExtensionHandle K2_RegisterExtensionAsDataForContext(FGameplayTag ExtensionPointTag, UObject* ContextObject,
	                                                        UObject* Data, int32 Priority = -1);

	// 根据内部 FUIExtension 构造一个对外通知使用的 FUIExtensionRequest。
	FUIExtensionRequest CreateExtensionRequest(const TSharedPtr<FUIExtension>& Extension);

private:
	// 一个 ExtensionPointTag 下可以同时存在多个 ExtensionPoint。
	typedef TArray<TSharedPtr<FUIExtensionPoint>> FExtensionPointList;

	// 按 GameplayTag 保存当前 World 中所有已经注册的 ExtensionPoint。
	TMap<FGameplayTag, FExtensionPointList> ExtensionPointMap;

	// 一个 ExtensionPointTag 下可以同时注册多个 Extension。
	typedef TArray<TSharedPtr<FUIExtension>> FExtensionList;

	// 按 GameplayTag 保存当前 World 中所有已经注册的 Extension。
	TMap<FGameplayTag, FExtensionList> ExtensionMap;
};


// FUIExtensionHandle 的蓝图辅助函数库。
// 让蓝图能够方便地注销 Extension 和判断 Handle 是否有效。
UCLASS()
class HODGEPODGE_API UUIExtensionHandleFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UUIExtensionHandleFunctions()
	{
	}

	// 通过 Handle 注销对应 Extension。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static void Unregister(UPARAM(ref) FUIExtensionHandle& Handle);

	// 判断当前 Extension Handle 是否有效。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static bool IsValid(UPARAM(ref) FUIExtensionHandle& Handle);
};

// FUIExtensionPointHandle 的蓝图辅助函数库。
// 让蓝图能够方便地注销 ExtensionPoint 和判断 Handle 是否有效。
UCLASS()
class HODGEPODGE_API UUIExtensionPointHandleFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UUIExtensionPointHandleFunctions()
	{
	}

	// 通过 Handle 注销对应 ExtensionPoint。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static void Unregister(UPARAM(ref) FUIExtensionPointHandle& Handle);

	// 判断当前 ExtensionPoint Handle 是否有效。
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "UI Extension")
	static bool IsValid(UPARAM(ref) FUIExtensionPointHandle& Handle);
};
