#pragma once

#include "Components/ActorComponent.h"
#include "NBPendingTaskClientComponent.generated.h"

/**
 * 로컬 연출의 완료 신호를 서버로 전달합니다.
 * 클라이언트가 소유한 PlayerController에 추가해야 Server RPC를 사용할 수 있습니다.
 */
UCLASS(ClassGroup = (NumberBaseball), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UNBPendingTaskClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBPendingTaskClientComponent();

	/** Widget이나 연출 객체에서 로컬 연출이 끝났을 때 호출합니다. */
	UFUNCTION(BlueprintCallable, Category = "PendingTask")
	void NotifyPendingTaskFinished(int32 TaskId);

private:
	/** 서버의 PendingTaskComponent로 완료 응답을 전달합니다. */
	UFUNCTION(Server, Reliable)
	void ServerNotifyPendingTaskFinished(int32 TaskId);
};
