#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "NBPendingTaskComponent.generated.h"

class ANBGameState;
class APlayerController;

/**
 * 서버에서 클라이언트의 작업 완료 응답을 기다립니다.
 * 지정된 클라이언트가 응답하거나 제한 시간이 지나면 등록된 Delegate를 한 번 실행합니다.
 */
UCLASS(ClassGroup = (NumberBaseball), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UNBPendingTaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBPendingTaskComponent();

	/** 새로운 작업을 시작합니다. 이미 진행 중인 작업이 있으면 시작하지 않습니다. */
	bool StartPendingTask(
		FGameplayTag TaskTag,
		APlayerController* CompletionPlayer,
		float MaxWaitTime,
		FSimpleDelegate OnCompleted);

	/** PlayerController가 전달한 완료 응답을 검증합니다. */
	void NotifyPendingTaskFinished(
		APlayerController* PlayerController,
		int32 TaskId);

	bool IsPendingTaskActive() const { return ActiveTaskId != INDEX_NONE; }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	ANBGameState* GetNBGameState() const;

	/** 작업 상태를 종료하고 보관 중인 Delegate를 실행합니다. */
	void CompletePendingTask(int32 TaskId);
	void HandlePendingTaskTimeout();

	/** 현재 작업을 완료할 수 있는 PlayerController입니다. */
	TWeakObjectPtr<APlayerController> CompletionPlayer;

	FSimpleDelegate PendingTaskCompletedDelegate;
	FTimerHandle PendingTaskTimerHandle;
	FGameplayTag ActiveTaskTag;
	int32 ActiveTaskId = INDEX_NONE;
	int32 NextTaskId = 0;
};
