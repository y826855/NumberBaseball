#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NBRoundScoreWidget.generated.h"

class UTextBlock;
struct FGameplayTag;
struct FNBRoundStateChangedMessage;
struct FNBScoreChangedMessage;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBRoundScoreWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoundText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UFUNCTION(BlueprintImplementableEvent, Category = "RoundScore")
	void OnRoundUpdated(int32 CurrentRound, int32 TotalRoundCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "RoundScore")
	void OnScoresUpdated();

private:
	void UpdateRound(int32 CurrentRound, int32 TotalRoundCount);
	void UpdateScores();
	void HandleRoundStateChanged(
		FGameplayTag Channel,
		const FNBRoundStateChangedMessage& Message);
	void HandleScoreChanged(
		FGameplayTag Channel,
		const FNBScoreChangedMessage& Message);

	FGameplayMessageListenerHandle RoundStateChangedListenerHandle;
	FGameplayMessageListenerHandle ScoreChangedListenerHandle;
};
