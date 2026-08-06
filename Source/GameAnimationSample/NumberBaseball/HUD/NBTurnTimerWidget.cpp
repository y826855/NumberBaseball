#include "NBTurnTimerWidget.h"

#include "Components/ProgressBar.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"
#include "GameFramework/GameStateBase.h"

void UNBTurnTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TimerChangedListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBTimerMessage>(
			NBGameplayMessages::TimerChanged,
			this,
			&ThisClass::HandleTimerChanged);
	TimerProgressBar->SetPercent(0.f);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNBTurnTimerWidget::NativeDestruct()
{
	TimerChangedListenerHandle.Unregister();
	Super::NativeDestruct();
}

void UNBTurnTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const AGameStateBase* GameState = GetWorld()->GetGameState();
	const float RemainingTime = ActiveTimerMessage.bIsActive && IsValid(GameState)
		? FMath::Max(
			0.f,
			ActiveTimerMessage.EndServerTime - GameState->GetServerWorldTimeSeconds())
		: 0.f;
	const bool bIsTimerActive = ActiveTimerMessage.bIsActive && RemainingTime > 0.f;
	const float Progress = ActiveTimerMessage.Duration > 0.f
		? FMath::Clamp(RemainingTime / ActiveTimerMessage.Duration, 0.f, 1.f)
		: 0.f;

	TimerProgressBar->SetPercent(Progress);
	OnTurnTimeUpdated(RemainingTime, bIsTimerActive);

	if (ActiveTimerMessage.bIsActive && bIsTimerActive == false)
	{
		ActiveTimerMessage.bIsActive = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UNBTurnTimerWidget::HandleTimerChanged(
	FGameplayTag Channel,
	const FNBTimerMessage& Message)
{
	ActiveTimerMessage = Message;
	SetVisibility(Message.bIsActive
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Collapsed);

	if (Message.bIsActive == false)
	{
		TimerProgressBar->SetPercent(0.f);
		OnTurnTimeUpdated(0.f, false);
	}
}
