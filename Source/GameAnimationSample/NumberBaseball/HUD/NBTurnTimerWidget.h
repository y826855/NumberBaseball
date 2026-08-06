#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBClientMessages.h"
#include "NBTurnTimerWidget.generated.h"

class UProgressBar;
struct FGameplayTag;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBTurnTimerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> TimerProgressBar;

	UFUNCTION(BlueprintImplementableEvent, Category = "TurnTimer")
	void OnTurnTimeUpdated(float RemainingTime, bool bIsTimerActive);

private:
	void HandleTimerChanged(FGameplayTag Channel, const FNBTimerMessage& Message);

	FNBTimerMessage ActiveTimerMessage;
	FGameplayMessageListenerHandle TimerChangedListenerHandle;
};
