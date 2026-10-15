/** @file HodgeHavocRules.h @brief 无UObject依赖的燃料时间与角色级拢音限流规则。 */
#pragma once
#include "CoreMinimal.h"

/** 每次会话独立的结算时钟；相同时间多次回调只结算一次。 */
struct HODGEPODGE_API FHodgeHavocFuelClock
{
	/** 启动时不扣100点；时间必须与服务器世界时间同源。 */
	void Start(double Now) { LastTime=Now; bStarted=FMath::IsFinite(Now); }
	/** 返回本段实际消耗；回退、非法时间与非法速度不会推进时钟。 */
	float Settle(double Now, float Rate);
	/** 会话失效后旧周期回调不再结算。 */
	void Reset() { bStarted=false; LastTime=0.; }
private:
	double LastTime=0.;
	bool bStarted=false;
};

/** 记录属于角色规则而非单个攻击实例，跨连段共享一秒限流。 */
struct HODGEPODGE_API FHodgeHavocProcGate
{
	/** 先占用再应用收益，属性广播重入不能再次触发。 */
	bool TryReserve(double Now, double Interval);
	/** 只在Avatar生命周期重置，不能因切换普攻段数清除。 */
	void Reset() { bReserved=false; LastTime=0.; }
private:
	double LastTime=0.;
	bool bReserved=false;
};

/** 元素倍率只作用于明确的湮灭伤害，非法加成安全退回原伤害。 */
struct HODGEPODGE_API FHodgeHavocDamageRules
{
	static float GetMultiplier(bool bHavoc, float Bonus);
};

/** 只负责一次长按识别，按下时普攻仍立即交给既有输入链。 */
struct HODGEPODGE_API FHodgeHavocHoldSession
{
	/** 返回是否新建了按键会话，重复Started不能重置计时。 */
	bool Begin(double Now);
	/** 只读判断阈值是否达到，不在松开时补发普通攻击。 */
	bool IsReady(double Now,float Threshold) const;
	/** 计时错误或有界等待超时后放弃本次输入。 */
	bool HasExpired(double Now,float Threshold,float Buffer) const;
	/** 一次按住最多提交一次请求。 */
	void Release() { bHeld=false; StartTime=0.; }
	bool IsHeld() const { return bHeld; }
private:
	double StartTime=0.;
	bool bHeld=false;
};
