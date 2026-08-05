#include "NBFireAbility.h"

#include "Engine/World.h"
#include "GameAnimationSample/NumberBaseball/AbilitySystem/NBCombatTags.h"
#include "GameAnimationSample/NumberBaseball/Character/Components/NBCombatComponent.h"
#include "GameAnimationSample/NumberBaseball/Projectile/NBProjectile.h"
#include "GameFramework/Pawn.h"

UNBFireAbility::UNBFireAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ActivationBlockedTags.AddTag(NBCombatTags::KnockBack);
}

bool UNBFireAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(
		Handle,
		ActorInfo,
		SourceTags,
		TargetTags,
		OptionalRelevantTags) == false)
	{
		return false;
	}

	const AActor* AvatarActor = ActorInfo != nullptr ? ActorInfo->AvatarActor.Get() : nullptr;
	const UNBCombatComponent* CombatComponent =
		IsValid(AvatarActor)
			? AvatarActor->FindComponentByClass<UNBCombatComponent>()
			: nullptr;
	return ProjectileClass != nullptr &&
		IsValid(CombatComponent) &&
		CombatComponent->CanExecuteFireAbility();
}

void UNBFireAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AActor* AvatarActor = ActorInfo != nullptr ? ActorInfo->AvatarActor.Get() : nullptr;
	UNBCombatComponent* CombatComponent =
		IsValid(AvatarActor)
			? AvatarActor->FindComponentByClass<UNBCombatComponent>()
			: nullptr;

	FTransform SpawnTransform;
	if (IsValid(CombatComponent) == false ||
		CombatComponent->ConsumeFireRequest(SpawnTransform) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	SpawnProjectile(AvatarActor, SpawnTransform);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UNBFireAbility::SpawnProjectile(
	AActor* AvatarActor,
	const FTransform& SpawnTransform) const
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false || IsValid(AvatarActor) == false || ProjectileClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = AvatarActor;
	SpawnParameters.Instigator = Cast<APawn>(AvatarActor);

	World->SpawnActor<ANBProjectile>(
		ProjectileClass,
		SpawnTransform,
		SpawnParameters);
}
