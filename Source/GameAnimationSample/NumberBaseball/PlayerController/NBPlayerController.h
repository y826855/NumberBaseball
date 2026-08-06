#pragma once

#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBPendingTaskClientComponent;
class UNBClientMessageComponent;
class UNBNotificationWidget;
class UNBNumberPadWidget;
class UNBRoundScoreWidget;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANBPlayerController();

	UFUNCTION(BlueprintPure, Category = "PendingTask")
	UNBPendingTaskClientComponent* GetPendingTaskClientComponent() const
	{
		return PendingTaskClientComponent;
	}

	UNBClientMessageComponent* GetClientMessageComponent() const
	{
		return ClientMessageComponent;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingTask")
	TObjectPtr<UNBPendingTaskClientComponent> PendingTaskClientComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ClientMessage")
	TObjectPtr<UNBClientMessageComponent> ClientMessageComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNBNumberPadWidget> NumberPadWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UNBNumberPadWidget> NumberPadWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNBNotificationWidget> NotificationWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UNBNotificationWidget> NotificationWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNBRoundScoreWidget> RoundScoreWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UNBRoundScoreWidget> RoundScoreWidget;
};
