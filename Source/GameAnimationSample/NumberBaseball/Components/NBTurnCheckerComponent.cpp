#include "NBTurnCheckerComponent.h"

#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

UNBTurnCheckerComponent::UNBTurnCheckerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNBTurnCheckerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	TurnPlayerChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBTurnPlayerChangedMessage>(
			NBGameplayMessages::TurnPlayerChanged,
			this,
			&ThisClass::HandleTurnPlayerChanged);
	RefreshTurnState();
}

void UNBTurnCheckerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TurnPlayerChangedListenerHandle.Unregister();
	Super::EndPlay(EndPlayReason);
}

bool UNBTurnCheckerComponent::IsOwnerTurn() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const ANBGameState* NBGameState = GetWorld()->GetGameState<ANBGameState>();
	return IsValid(OwnerPawn)
		&& IsValid(NBGameState)
		&& OwnerPawn->GetPlayerState() == NBGameState->GetCurrentTurnPlayer();
}

void UNBTurnCheckerComponent::RefreshTurnState()
{
	OnOwnerTurnChanged.Broadcast(IsOwnerTurn());
}

void UNBTurnCheckerComponent::HandleTurnPlayerChanged(
	FGameplayTag Channel,
	const FNBTurnPlayerChangedMessage& Message)
{
	RefreshTurnState();
}
