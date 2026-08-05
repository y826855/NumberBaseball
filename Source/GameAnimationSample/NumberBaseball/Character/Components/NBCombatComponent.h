#pragma once

#include "Components/ActorComponent.h"
#include "NBCombatComponent.generated.h"

class UGameplayAbility;
class UNBFireAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FNBAimingChangedSignature,
	bool,
	bIsAiming);

UCLASS(ClassGroup = (NumberBaseball), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UNBCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBCombatComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAiming(bool bNewIsAiming);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TryFire(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool CanUseCombat() const;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FNBAimingChangedSignature OnAimingChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UGameplayAbility> FireAbilityClass;

private:
	friend class UNBFireAbility;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bNewIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerTryFire(const FTransform& SpawnTransform);

	void ApplyAiming(bool bNewIsAiming);
	void ActivateFireAbility(const FTransform& SpawnTransform);
	bool CanExecuteFireAbility() const;
	bool ConsumeFireRequest(FTransform& OutSpawnTransform);

	bool bIsAiming = false;
	bool bHasPendingFire = false;
	FTransform PendingFireSpawnTransform;
};
