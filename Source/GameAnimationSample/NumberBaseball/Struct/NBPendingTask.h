#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NBPendingTask.generated.h"

USTRUCT(BlueprintType)
struct GAMEANIMATIONSAMPLE_API FNBPendingTaskState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PendingTask")
	int32 TaskId = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "PendingTask")
	FGameplayTag TaskTag;

	UPROPERTY(BlueprintReadOnly, Category = "PendingTask")
	bool bIsActive = false;
};
