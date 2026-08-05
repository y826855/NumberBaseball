#include "NBCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameAnimationSample/NumberBaseball/AbilitySystem/Abilities/NBFireAbility.h"
#include "GameAnimationSample/NumberBaseball/AbilitySystem/NBCombatTags.h"
#include "GameplayAbilitySpec.h"

UNBCombatComponent::UNBCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	FireAbilityClass = UNBFireAbility::StaticClass();
}

void UNBCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority() == false || FireAbilityClass == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (IsValid(AbilitySystem) &&
		AbilitySystem->FindAbilitySpecFromClass(FireAbilityClass) == nullptr)
	{
		AbilitySystem->GiveAbility(FGameplayAbilitySpec(FireAbilityClass, 1));
	}
}

void UNBCombatComponent::SetAiming(bool bNewIsAiming)
{
	if (bNewIsAiming && CanUseCombat() == false)
	{
		return;
	}

	ApplyAiming(bNewIsAiming);

	if (GetOwner()->HasAuthority() == false)
	{
		ServerSetAiming(bNewIsAiming);
	}
}

void UNBCombatComponent::TryFire(const FTransform& SpawnTransform)
{
	if (bIsAiming == false || FireAbilityClass == nullptr)
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		ActivateFireAbility(SpawnTransform);
		return;
	}

	ServerTryFire(SpawnTransform);
}

void UNBCombatComponent::ServerSetAiming_Implementation(bool bNewIsAiming)
{
	if (bNewIsAiming == false || CanUseCombat())
	{
		ApplyAiming(bNewIsAiming);
	}
}

void UNBCombatComponent::ServerTryFire_Implementation(const FTransform& SpawnTransform)
{
	if (bIsAiming)
	{
		ActivateFireAbility(SpawnTransform);
	}
}

bool UNBCombatComponent::CanUseCombat() const
{
	UAbilitySystemComponent* AbilitySystem =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	return IsValid(AbilitySystem) == false ||
		AbilitySystem->HasMatchingGameplayTag(NBCombatTags::KnockBack) == false;
}

void UNBCombatComponent::ActivateFireAbility(const FTransform& SpawnTransform)
{
	UAbilitySystemComponent* AbilitySystem =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (IsValid(AbilitySystem) == false || FireAbilityClass == nullptr)
	{
		return;
	}

	PendingFireSpawnTransform = SpawnTransform;
	bHasPendingFire = true;

	if (AbilitySystem->TryActivateAbilityByClass(FireAbilityClass) == false)
	{
		bHasPendingFire = false;
	}
}

bool UNBCombatComponent::CanExecuteFireAbility() const
{
	return bIsAiming && bHasPendingFire;
}

bool UNBCombatComponent::ConsumeFireRequest(FTransform& OutSpawnTransform)
{
	if (CanExecuteFireAbility() == false)
	{
		return false;
	}

	OutSpawnTransform = PendingFireSpawnTransform;
	bHasPendingFire = false;
	return true;
}

void UNBCombatComponent::ApplyAiming(bool bNewIsAiming)
{
	if (bIsAiming == bNewIsAiming)
	{
		return;
	}

	bIsAiming = bNewIsAiming;
	OnAimingChanged.Broadcast(bIsAiming);
}
