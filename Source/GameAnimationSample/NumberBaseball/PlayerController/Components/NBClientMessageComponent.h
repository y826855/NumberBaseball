#pragma once

#include "Components/ActorComponent.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBClientMessages.h"
#include "TimerManager.h"
#include "NBClientMessageComponent.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UNBClientMessageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBClientMessageComponent();

	void SendTimerMessage(const FNBTimerMessage& Message);
	void SendNotifyMessage(const FNBNotifyMessage& Message);
	bool GetLatestNotifyMessage(FNBNotifyMessage& OutMessage) const;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION(Client, Reliable)
	void ClientReceiveTimerMessage(const FNBTimerMessage& Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveNotifyMessage(const FNBNotifyMessage& Message);

	void LogTimer();

	FNBTimerMessage ActiveTimerMessage;
	FNBNotifyMessage LatestNotifyMessage;
	bool bHasNotifyMessage = false;
	float LatestNotifyExpirationTime = 0.f;
	FTimerHandle TimerLogHandle;
};
