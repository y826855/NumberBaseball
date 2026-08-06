#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGuessResult.h"
#include "TimerManager.h"
#include "NBNumberPadWidget.generated.h"

struct FGameplayTag;
struct FNBInputValuesChangedMessage;
struct FNBPendingTaskState;
struct FNBRoundStateChangedMessage;
struct FNBTurnPhaseChangedMessage;
class UPanelWidget;
class UNBNumberWidget;
class UNBResultWidget;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBNumberPadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NumberPad")
	void FinishResultPresentation();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> UpperBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> LowerBox;

	UPROPERTY(EditDefaultsOnly, Category = "NumberPad")
	TSubclassOf<UNBNumberWidget> NumberWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "NumberPad")
	TSubclassOf<UNBResultWidget> ResultWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "NumberPad", meta = (ClampMin = "0.0"))
	float ResultPresentationDuration = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 DisplayedInputCount = 0;
	
private:
	void CreateRoundWidgets(int32 RequiredInputCount);
	void ClearRoundWidgets();
	void ResetTurnWidgets();
	void ApplyInputValues(const TArray<int32>& InputValues);
	void ApplyGuessResult(const FNBGuessResult& GuessResult);

	void HandleInputValuesChanged(
		FGameplayTag Channel,
		const FNBInputValuesChangedMessage& Message);
	void HandleTurnPhaseChanged(
		FGameplayTag Channel,
		const FNBTurnPhaseChangedMessage& Message);
	void HandleRoundStateChanged(
		FGameplayTag Channel,
		const FNBRoundStateChangedMessage& Message);
	void HandleGuessResultChanged(
		FGameplayTag Channel,
		const FNBGuessResult& GuessResult);
	void HandlePendingTaskChanged(
		FGameplayTag Channel,
		const FNBPendingTaskState& PendingTaskState);

	FGameplayMessageListenerHandle InputValuesChangedListenerHandle;
	FGameplayMessageListenerHandle TurnPhaseChangedListenerHandle;
	FGameplayMessageListenerHandle RoundStateChangedListenerHandle;
	FGameplayMessageListenerHandle GuessResultChangedListenerHandle;
	FGameplayMessageListenerHandle PendingTaskChangedListenerHandle;
	UPROPERTY(Transient)
	TArray<TObjectPtr<UNBNumberWidget>> NumberWidgets;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNBResultWidget>> ResultWidgets;

	FTimerHandle ResultPresentationTimerHandle;
	int32 ActivePendingTaskId = INDEX_NONE;
};
