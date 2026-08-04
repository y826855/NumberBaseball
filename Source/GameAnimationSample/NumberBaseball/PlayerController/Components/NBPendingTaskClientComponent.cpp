#include "NBPendingTaskClientComponent.h"

#include "GameAnimationSample/NumberBaseball/Core/Components/NBPendingTaskComponent.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameMode.h"
#include "GameFramework/PlayerController.h"

UNBPendingTaskClientComponent::UNBPendingTaskClientComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNBPendingTaskClientComponent::NotifyPendingTaskFinished(int32 TaskId)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (IsValid(PlayerController) == false || PlayerController->IsLocalController() == false)
	{
		return;
	}

	if (PlayerController->HasAuthority())
	{
		ServerNotifyPendingTaskFinished_Implementation(TaskId);
		return;
	}

	ServerNotifyPendingTaskFinished(TaskId);
}

void UNBPendingTaskClientComponent::ServerNotifyPendingTaskFinished_Implementation(int32 TaskId)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	UWorld* World = GetWorld();
	if (IsValid(PlayerController) == false || IsValid(World) == false)
	{
		return;
	}

	if (ANBGameMode* NBGameMode = Cast<ANBGameMode>(World->GetAuthGameMode()))
	{
		NBGameMode->GetPendingTaskComponent()->NotifyPendingTaskFinished(
			PlayerController,
			TaskId);
	}
}
