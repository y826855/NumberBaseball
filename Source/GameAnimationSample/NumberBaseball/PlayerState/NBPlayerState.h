#pragma once

#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddCorrectAnswer();
	void ResetCorrectAnswerCount();

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetCorrectAnswerCount() const { return CorrectAnswerCount; }

protected:
	virtual void OnRep_PlayerName() override;

private:
	UFUNCTION()
	void OnRep_CorrectAnswerCount();

	UPROPERTY(ReplicatedUsing = OnRep_CorrectAnswerCount)
	int32 CorrectAnswerCount = 0;
};
