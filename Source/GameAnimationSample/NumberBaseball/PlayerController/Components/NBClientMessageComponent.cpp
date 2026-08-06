#include "NBClientMessageComponent.h"

#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogNBClientMessage, Log, All);

UNBClientMessageComponent::UNBClientMessageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNBClientMessageComponent::SendTimerMessage(const FNBTimerMessage& Message)
{
	if (IsValid(GetOwner()) && GetOwner()->HasAuthority())
	{
		ClientReceiveTimerMessage(Message);
	}
}

void UNBClientMessageComponent::SendNotifyMessage(const FNBNotifyMessage& Message)
{
	if (IsValid(GetOwner()) && GetOwner()->HasAuthority())
	{
		ClientReceiveNotifyMessage(Message);
	}
}

bool UNBClientMessageComponent::GetLatestNotifyMessage(FNBNotifyMessage& OutMessage) const
{
	if (bHasNotifyMessage == false)
	{
		return false;
	}

	OutMessage = LatestNotifyMessage;
	if (LatestNotifyExpirationTime > 0.f)
	{
		OutMessage.DisplayDuration = LatestNotifyExpirationTime - GetWorld()->GetTimeSeconds();
		if (OutMessage.DisplayDuration <= 0.f)
		{
			return false;
		}
	}
	return true;
}

void UNBClientMessageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerLogHandle);
	Super::EndPlay(EndPlayReason);
}

void UNBClientMessageComponent::ClientReceiveTimerMessage_Implementation(
	const FNBTimerMessage& Message)
{
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::TimerChanged,
		Message);

	GetWorld()->GetTimerManager().ClearTimer(TimerLogHandle);
	ActiveTimerMessage = Message;

	if (Message.bIsActive == false)
	{
		return;
	}

	LogTimer();
	GetWorld()->GetTimerManager().SetTimer(
		TimerLogHandle,
		this,
		&ThisClass::LogTimer,
		1.f,
		true);
}

void UNBClientMessageComponent::ClientReceiveNotifyMessage_Implementation(
	const FNBNotifyMessage& Message)
{
	LatestNotifyMessage = Message;
	bHasNotifyMessage = true;
	LatestNotifyExpirationTime = Message.bIsActive && Message.DisplayDuration > 0.f
		? GetWorld()->GetTimeSeconds() + Message.DisplayDuration
		: 0.f;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(
		NBGameplayMessages::NotifyReceived,
		Message);
}

void UNBClientMessageComponent::LogTimer()
{
	const AGameStateBase* GameState = GetWorld()->GetGameState();
	if (IsValid(GameState) == false)
	{
		return;
	}

	const float RemainingTime = FMath::Max(
		0.f,
		ActiveTimerMessage.EndServerTime - GameState->GetServerWorldTimeSeconds());

	UE_LOG(LogNBClientMessage, Display,
		TEXT("Timer remaining: %d"),
		FMath::CeilToInt(RemainingTime));

	if (RemainingTime <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerLogHandle);
	}
}
