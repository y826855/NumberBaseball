#include "NBPendingTaskComponent.h"

#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/Core/NBGameState.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBPendingTask.h"
#include "GameFramework/PlayerController.h"

UNBPendingTaskComponent::UNBPendingTaskComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UNBPendingTaskComponent::StartPendingTask(
	FGameplayTag TaskTag,
	APlayerController* InCompletionPlayer,
	float MaxWaitTime,
	FSimpleDelegate OnCompleted)
{
	ANBGameState* NBGameState = GetNBGameState();
	if (IsPendingTaskActive()
		|| IsValid(NBGameState) == false
		|| IsValid(InCompletionPlayer) == false)
	{
		return false;
	}

	ActiveTaskId = ++NextTaskId;
	ActiveTaskTag = TaskTag;
	CompletionPlayer = InCompletionPlayer;
	PendingTaskCompletedDelegate = MoveTemp(OnCompleted);

	GetWorld()->GetTimerManager().SetTimer(
		PendingTaskTimerHandle,
		this,
		&ThisClass::HandlePendingTaskTimeout,
		FMath::Max(0.1f, MaxWaitTime),
		false);

	FNBPendingTaskState PendingTaskState;
	PendingTaskState.TaskId = ActiveTaskId;
	PendingTaskState.TaskTag = ActiveTaskTag;
	PendingTaskState.bIsActive = true;
	NBGameState->SetPendingTaskState(PendingTaskState);

	return true;
}

void UNBPendingTaskComponent::NotifyPendingTaskFinished(
	APlayerController* PlayerController,
	int32 TaskId)
{
	if (PlayerController != CompletionPlayer.Get())
	{
		return;
	}

	CompletePendingTask(TaskId);
}

void UNBPendingTaskComponent::NotifyPlayerDisconnected(
	APlayerController* PlayerController)
{
	if (IsPendingTaskActive()
		&& IsValid(PlayerController)
		&& PlayerController == CompletionPlayer.Get())
	{
		CompletePendingTask(ActiveTaskId);
	}
}

void UNBPendingTaskComponent::CancelPendingTask()
{
	if (IsPendingTaskActive() == false)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(PendingTaskTimerHandle);
	ActiveTaskId = INDEX_NONE;
	ActiveTaskTag = FGameplayTag();
	CompletionPlayer.Reset();
	PendingTaskCompletedDelegate.Unbind();

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->SetPendingTaskState(FNBPendingTaskState());
	}
}

void UNBPendingTaskComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(PendingTaskTimerHandle);
	PendingTaskCompletedDelegate.Unbind();

	Super::EndPlay(EndPlayReason);
}

ANBGameState* UNBPendingTaskComponent::GetNBGameState() const
{
	UWorld* World = GetWorld();
	return IsValid(World) ? World->GetGameState<ANBGameState>() : nullptr;
}

void UNBPendingTaskComponent::CompletePendingTask(int32 TaskId)
{
	if (TaskId != ActiveTaskId)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(PendingTaskTimerHandle);

	FNBPendingTaskState PendingTaskState;
	PendingTaskState.TaskId = ActiveTaskId;
	PendingTaskState.TaskTag = ActiveTaskTag;

	ActiveTaskId = INDEX_NONE;
	ActiveTaskTag = FGameplayTag();
	CompletionPlayer.Reset();

	FSimpleDelegate CompletedDelegate = MoveTemp(PendingTaskCompletedDelegate);
	PendingTaskCompletedDelegate.Unbind();

	if (ANBGameState* NBGameState = GetNBGameState())
	{
		NBGameState->SetPendingTaskState(PendingTaskState);
	}

	CompletedDelegate.ExecuteIfBound();
}

void UNBPendingTaskComponent::HandlePendingTaskTimeout()
{
	CompletePendingTask(ActiveTaskId);
}
