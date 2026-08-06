#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NBClientMessages.generated.h"

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBTimerMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	FGameplayTag TimerType;

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	float Duration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	float EndServerTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	bool bIsActive = false;
};

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBNotifyMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Notify")
	FGameplayTag MessageType;

	UPROPERTY(BlueprintReadOnly, Category = "Notify")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category = "Notify")
	float DisplayDuration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Notify")
	bool bIsActive = true;
};
