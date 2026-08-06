#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBClientMessages.h"
#include "TimerManager.h"
#include "NBNotificationWidget.generated.h"

class UTextBlock;
struct FGameplayTag;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;

	UFUNCTION(BlueprintImplementableEvent, Category = "Notify")
	void OnNotificationReceived(const FNBNotifyMessage& Message);

private:
	void HandleNotifyReceived(FGameplayTag Channel, const FNBNotifyMessage& Message);
	void HideNotification();

	FGameplayMessageListenerHandle NotifyListenerHandle;
	FTimerHandle HideTimerHandle;
};
