#pragma once

#include "GameFramework/Actor.h"
#include "NBNumberPad.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class USceneComponent;
class APawn;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBNumberPad : public AActor
{
	GENERATED_BODY()

public:
	ANBNumberPad();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Number Pad")
	void CompletePress();

	UFUNCTION(BlueprintCallable, Category = "Number Pad")
	void ResetInput();

	UFUNCTION(BlueprintPure, Category = "Number Pad")
	bool CanSubmitInput() const { return bIsPadEnabled && bInputSubmitted == false; }

	void SetPadEnabled(bool bEnabled);
	void SetPadNumber(int32 Number);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Number Pad")
	void OnPressStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Number Pad")
	void OnPressReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "Number Pad")
	void OnInputSubmittedChanged(bool bIsSubmitted);

	UFUNCTION(BlueprintImplementableEvent, Category = "Number Pad")
	void OnPadNumberChanged(int32 Number);

	UFUNCTION(BlueprintImplementableEvent, Category = "Number Pad")
	void OnPadActivated();

	UPROPERTY(ReplicatedUsing = OnRep_InputSubmitted, BlueprintReadOnly, Category = "Number Pad")
	bool bInputSubmitted = false;

	UPROPERTY(ReplicatedUsing = OnRep_HavingNumber, BlueprintReadOnly, Category = "Number Pad")
	int32 HavingNumber = 0;

	UPROPERTY(ReplicatedUsing = OnRep_IsPressed, BlueprintReadOnly, Category = "Number Pad")
	bool bIsPressed = false;

private:
	void SetPressed(bool bPressed);

	UFUNCTION()
	void OnRep_InputSubmitted();

	UFUNCTION()
	void OnRep_HavingNumber();

	UFUNCTION()
	void OnRep_IsPadEnabled();

	UFUNCTION()
	void OnRep_IsPressed();

	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(ReplicatedUsing = OnRep_IsPadEnabled)
	bool bIsPadEnabled = true;
};
