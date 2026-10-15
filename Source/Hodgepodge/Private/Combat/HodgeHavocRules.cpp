/** @file HodgeHavocRules.cpp @brief 燃料按实际时间消耗，增益与资源计时互不混用。 */
#include "Combat/HodgeHavocRules.h"

float FHodgeHavocFuelClock::Settle(double Now, float Rate)
{
	if (!bStarted || !FMath::IsFinite(Now) || !FMath::IsFinite(Rate) || Rate<=0.f || Now<LastTime) { return 0.f; }
	const double Amount=(Now-LastTime)*Rate;
	if (!FMath::IsFinite(Amount) || Amount>MAX_flt) { return 0.f; }
	LastTime=Now; return static_cast<float>(Amount);
}

bool FHodgeHavocProcGate::TryReserve(double Now, double Interval)
{
	if (!FMath::IsFinite(Now) || !FMath::IsFinite(Interval) || Interval<=0. || (bReserved && Now-LastTime+UE_DOUBLE_SMALL_NUMBER<Interval)) { return false; }
	bReserved=true; LastTime=Now; return true;
}

float FHodgeHavocDamageRules::GetMultiplier(bool bHavoc, float Bonus)
{
	return bHavoc && FMath::IsFinite(Bonus) ? FMath::Max(0.f,1.f+Bonus) : 1.f;
}

bool FHodgeHavocHoldSession::Begin(double Now)
{
	if (bHeld || !FMath::IsFinite(Now)) { return false; }
	bHeld=true; StartTime=Now; return true;
}

bool FHodgeHavocHoldSession::IsReady(double Now,float Threshold) const
{
	return bHeld && FMath::IsFinite(Now) && FMath::IsFinite(Threshold) && Threshold>0.f && Now-StartTime>=Threshold;
}

bool FHodgeHavocHoldSession::HasExpired(double Now,float Threshold,float Buffer) const
{
	return !bHeld || !FMath::IsFinite(Now) || !FMath::IsFinite(Threshold) || Threshold<=0.f || !FMath::IsFinite(Buffer) || Buffer<0.f ||
		Now<StartTime || Now-StartTime>static_cast<double>(Threshold)+Buffer;
}
