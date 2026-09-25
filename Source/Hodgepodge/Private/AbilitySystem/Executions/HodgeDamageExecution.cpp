// 111屎山代码来袭


// UHodgeDamageExecution 类定义。
#include "AbilitySystem/Executions/HodgeDamageExecution.h"

// 项目自定义 GameplayEffectContext。
// 用于从伤害 Effect 中获取 AbilitySource、HitResult、物理材质等额外上下文。
#include "AbilitySystem/HodgeGameplayEffectContext.h"

// CombatSet 中定义 BaseDamage 等战斗基础属性。
#include "AbilitySystem/AttributeSet/HodgeCombatSet.h"

// HealthSet 中定义 Damage Meta Attribute，用于接收最终伤害计算结果。
#include "AbilitySystem/AttributeSet/HodgeHealthSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HodgeDamageExecution)

// 集中保存当前伤害 ExecutionCalculation 需要使用的 Attribute 捕获定义。
struct FDamageStatics
{
	// BaseDamage 的 Attribute 捕获定义。
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		// 捕获 Source ASC 上 CombatSet 的 BaseDamage 属性。
		// Source 表示该属性来自 GameplayEffect 的施加者。
		// 最后的 true 表示使用 Snapshot，在 GameplayEffectSpec 创建时捕获并固定该属性值。
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UHodgeCombatSet::GetBaseDamageAttribute(),
		                                                          EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

// 获取全局唯一的伤害属性捕获定义集合。
static FDamageStatics& DamageStatics()
{
	// 使用函数内静态对象，避免每次执行伤害计算时重复创建 CaptureDefinition。
	static FDamageStatics Statics;

	// 返回伤害计算使用的静态捕获定义。
	return Statics;
}


// 构造伤害 ExecutionCalculation。
UHodgeDamageExecution::UHodgeDamageExecution()
{
	// 告诉 GAS 当前 ExecutionCalculation 需要捕获 BaseDamage 属性。
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

// 执行实际的伤害数值计算。
void UHodgeDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                   FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	// 获取当前正在执行的 GameplayEffectSpec。
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 将标准 EffectContext 转换为项目自定义 FHodgeGameplayEffectContext。
	// 后续需要从中读取 AbilitySource、HitResult、物理材质和 Origin 等额外伤害上下文。
	FHodgeGameplayEffectContext* TypedContext = FHodgeGameplayEffectContext::ExtractEffectContext(Spec.GetContext());

	// 当前伤害 Execution 依赖项目自定义 EffectContext，因此这里要求转换结果必须有效。
	check(TypedContext);

	// 获取 GameplayEffectSpec 捕获到的 Source GameplayTag。
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	// 获取 GameplayEffectSpec 捕获到的 Target GameplayTag。
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 构造 Attribute 聚合器计算参数。
	// 捕获属性计算时可以根据 Source / Target Tag 应用对应的 Modifier。
	FAggregatorEvaluateParameters EvaluateParameters;

	// 将 Source Tag 提供给 Attribute 聚合器。
	EvaluateParameters.SourceTags = SourceTags;

	// 将 Target Tag 提供给 Attribute 聚合器。
	EvaluateParameters.TargetTags = TargetTags;

	// 保存从 Source 捕获并计算得到的基础伤害。
	float BaseDamage = 0.0f;

	// 根据 BaseDamageDef 和当前 Source / Target Tag，
	// 计算最终捕获到的 BaseDamage Magnitude。
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters,
	                                                           BaseDamage);

	// 获取真正造成当前 GameplayEffect 的 Actor，例如角色、武器或投射物。
	const AActor* EffectCauser = TypedContext->GetEffectCauser();

	// 从自定义 EffectContext 中获取本次攻击对应的 HitResult。
	const FHitResult* HitActorResult = TypedContext->GetHitResult();

	// 保存最终被命中的 Actor。
	AActor* HitActor = nullptr;

	// 保存命中位置。
	FVector ImpactLocation = FVector::ZeroVector;

	// 保存命中表面法线。
	FVector ImpactNormal = FVector::ZeroVector;

	// 保存 Trace 起点。
	FVector StartTrace = FVector::ZeroVector;

	// 保存 Trace 终点。
	FVector EndTrace = FVector::ZeroVector;

	// Calculation of hit actor, surface, zone, and distance all rely on whether the calculation has a hit result or not.
	// 命中 Actor、表面、命中区域以及距离等信息，优先依赖 EffectContext 中携带的 HitResult。

	// Effects just being added directly w/o having been targeted will always come in without a hit result, which must default
	// 如果 GameplayEffect 是直接应用到目标，而不是通过射线、投射物等命中产生，就可能没有 HitResult。

	// to some fallback information.
	// 因此没有 HitResult 时需要使用目标 ASC / AvatarActor 等信息作为兜底。

	// 如果当前 EffectContext 中存在有效 HitResult。
	if (HitActorResult)
	{
		// 获取当前命中结果。
		const FHitResult& CurHitResult = *HitActorResult;

		// 从 HitObjectHandle 中取得实际命中的 Actor。
		HitActor = CurHitResult.HitObjectHandle.FetchActor();

		// 成功获得命中 Actor 后保存完整命中空间信息。
		if (HitActor)
		{
			// 保存实际碰撞点。
			ImpactLocation = CurHitResult.ImpactPoint;

			// 保存碰撞点表面法线。
			ImpactNormal = CurHitResult.ImpactNormal;

			// 保存本次 Trace 起点。
			StartTrace = CurHitResult.TraceStart;

			// 保存本次 Trace 终点。
			EndTrace = CurHitResult.TraceEnd;
		}
	}

	// Handle case of no hit result or hit result not actually returning an actor
	// 获取当前 GameplayEffect 的目标 ASC，作为没有有效 HitActor 时的兜底来源。
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

	// 如果 HitResult 不存在，或者 HitResult 没有返回有效 Actor。
	if (!HitActor)
	{
		// 使用目标 ASC 的 AvatarActor 作为实际受击 Actor。
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

		// 如果成功获得目标 Actor。
		if (HitActor)
		{
			// 没有具体碰撞点时，使用目标 Actor 的位置作为命中位置。
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// Apply rules for team damage/self damage/etc...
	// 伤害交互倍率，用于处理敌我伤害、友军伤害、自伤等规则。
	// 0 表示禁止造成伤害，1 表示允许完整伤害，也可以扩展为其他倍率。
	float DamageInteractionAllowedMultiplier = 0.0f;

	// 只有存在有效受击 Actor 时才能判断双方伤害关系。
	if (HitActor)
	{
		// 原设计：通过 TeamSubsystem 判断 EffectCauser 是否允许对 HitActor 造成伤害。
		// UHodgeTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<UHodgeTeamSubsystem>();
		// if (ensure(TeamSubsystem))
		// {
		//     DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.0 : 0.0;
		// }
	}

	// Determine distance
	// 默认使用 WORLD_MAX，表示暂时无法确定攻击来源与命中位置之间的距离。
	double Distance = WORLD_MAX;

	// 如果 EffectContext 显式保存了攻击 Origin。
	if (TypedContext->HasOrigin())
	{
		// 优先使用 EffectContext Origin 到 ImpactLocation 的距离。
		Distance = FVector::Dist(TypedContext->GetOrigin(), ImpactLocation);
	}

	// 没有 Origin，但存在 EffectCauser。
	else if (EffectCauser)
	{
		// 使用 EffectCauser 当前世界坐标到命中位置之间的距离。
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}

	// Origin 和 EffectCauser 都不存在，无法推导攻击来源位置。
	else
	{
		// 输出错误日志，并继续使用 WORLD_MAX 作为距离兜底值。
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "Damage Calculation cannot deduce a source location for damage coming from %s; Falling back to WORLD_MAX dist!"
		       ), *GetPathNameSafe(Spec.Def));
	}

	// Apply ability source modifiers
	// 物理材质伤害衰减倍率，默认不进行衰减。
	float PhysicalMaterialAttenuation = 1.0f;

	// 距离伤害衰减倍率，默认不进行衰减。
	float DistanceAttenuation = 1.0f;

	// 如果当前 EffectContext 携带实现 IHodgeAbilitySourceInterface 的 AbilitySource。
	if (const IHodgeAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		// 如果当前命中结果能够提供物理材质。
		if (const UPhysicalMaterial* PhysMat = TypedContext->GetPhysicalMaterial())
		{
			// 根据物理材质以及 Source / Target Tag 计算物理材质伤害倍率。
			// 例如可以用于头部、护甲、肉体或其他材质对应的伤害修正。
			PhysicalMaterialAttenuation = AbilitySource->
				GetPhysicalMaterialAttenuation(PhysMat, SourceTags, TargetTags);
		}

		// 根据攻击距离以及 Source / Target Tag 计算距离伤害倍率。
		DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance, SourceTags, TargetTags);
	}

	// 防止距离衰减倍率出现负数，从而产生反向伤害等异常结果。
	DistanceAttenuation = FMath::Max(DistanceAttenuation, 0.0f);

	// Clamping is done when damage is converted to -health
	// 根据基础伤害、距离倍率、物理材质倍率以及伤害交互倍率计算最终伤害。
	// 最终 Health 的上下限 Clamp 会在 HealthSet 将 Damage 转换为 -Health 时统一处理。
	const float DamageDone = FMath::Max(
		BaseDamage * DistanceAttenuation * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);

	// 只有最终伤害大于 0 时才向 ExecutionOutput 写入伤害结果。
	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		// 将最终伤害以 Additive 方式写入目标 HealthSet 的 Damage Meta Attribute。
		// Damage 本身不是最终 Health，而会在 HealthSet::PostGameplayEffectExecute 中转换为 -Health。
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(UHodgeHealthSet::GetDamageAttribute(), EGameplayModOp::Additive,
			                               DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}
