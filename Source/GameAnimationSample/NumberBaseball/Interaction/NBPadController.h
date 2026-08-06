#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameAnimationSample/NumberBaseball/Core/NBTypes.h"
#include "NBPadController.generated.h"

class ANBNumberPad;
class USceneComponent;
struct FGameplayTag;
struct FNBTurnPhaseChangedMessage;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBPadController : public AActor
{
	GENERATED_BODY()

public:
	ANBPadController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Number Pad")
	void ResetPads();

	UFUNCTION(BlueprintCallable, Category = "Number Pad")
	void SetPadNumber(int32 PadIndex, int32 Number);

	UFUNCTION(BlueprintCallable, Category = "Number Pad")
	void SetControllerActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Number Pad")
	bool IsControllerActive() const { return bIsActive; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Number Pad")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Number Pad")
	TSubclassOf<ANBNumberPad> PadClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Number Pad", meta = (ClampMin = "0.0"))
	FVector2D Padding = FVector2D(200.f, 200.f);

	UFUNCTION(BlueprintImplementableEvent, Category = "Number Pad")
	void OnActiveStateChanged(bool bActive);

private:
	void SpawnPads();
	FTransform GetPadTransform(int32 PadIndex) const;
	void HandleTurnPhaseChanged(
		FGameplayTag Channel,
		const FNBTurnPhaseChangedMessage& Message);
	void ApplyTurnPhase(ENBTurnPhase TurnPhase);

	UFUNCTION()
	void OnRep_IsActive();

	UPROPERTY(Transient)
	TArray<TObjectPtr<ANBNumberPad>> SpawnedPads;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive = false;

	FGameplayMessageListenerHandle TurnPhaseChangedListenerHandle;
};
