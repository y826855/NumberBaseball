#pragma once

#include "Components/ActorComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NBTurnCheckerComponent.generated.h"

struct FGameplayTag;
struct FNBTurnPlayerChangedMessage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNBOnOwnerTurnChanged, bool, bIsOwnerTurn);

UCLASS(ClassGroup = (NumberBaseball), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UNBTurnCheckerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBTurnCheckerComponent();

	UFUNCTION(BlueprintPure, Category = "Turn")
	bool IsOwnerTurn() const;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FNBOnOwnerTurnChanged OnOwnerTurnChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void RefreshTurnState();
	void HandleTurnPlayerChanged(
		FGameplayTag Channel,
		const FNBTurnPlayerChangedMessage& Message);

	FGameplayMessageListenerHandle TurnPlayerChangedListenerHandle;
};
