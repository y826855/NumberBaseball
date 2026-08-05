#pragma once

#include "Abilities/GameplayAbility.h"
#include "NBFireAbility.generated.h"

class ANBProjectile;

UCLASS()
class GAMEANIMATIONSAMPLE_API UNBFireAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UNBFireAbility();

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<ANBProjectile> ProjectileClass;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	void SpawnProjectile(
		AActor* AvatarActor,
		const FTransform& SpawnTransform) const;
};
