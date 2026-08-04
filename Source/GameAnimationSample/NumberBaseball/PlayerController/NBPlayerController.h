#pragma once

#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBPendingTaskClientComponent;

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingTask")
	TObjectPtr<UNBPendingTaskClientComponent> PendingTaskClientComponent;
};
