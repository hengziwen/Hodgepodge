#include "Commands/UnrealMCPNiagaraCommands.h"
#include "Commands/UnrealMCPCommonUtils.h"
TSharedPtr<FJsonObject> FUnrealMCPNiagaraCommands::HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
    return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Niagara authoring is not supported in the Hodgepodge UE 5.5 blueprint build."));
}
