#include "NBPlayerController.h"

#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBPendingTaskClientComponent.h"

ANBPlayerController::ANBPlayerController()
{
	PendingTaskClientComponent = CreateDefaultSubobject<UNBPendingTaskClientComponent>(
		TEXT("PendingTaskClientComponent"));
}
