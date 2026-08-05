#include "NBKnockbackComponent.h"

#include "DefaultMovementSet/LayeredMoves/LaunchMove.h"
#include "GameFramework/Character.h"
#include "MoverComponent.h"

UNBKnockbackComponent::UNBKnockbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNBKnockbackComponent::ApplyKnockback(
	const FVector& Direction,
	float Strength)
{
	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == false)
	{
		return;
	}

	if (UMoverComponent* MoverComponent = OwnerActor->FindComponentByClass<UMoverComponent>())
	{
		TSharedPtr<FLayeredMove_Launch> LaunchMove = MakeShared<FLayeredMove_Launch>();
		LaunchMove->LaunchVelocity = Direction.GetSafeNormal() * Strength;
		MoverComponent->QueueLayeredMove(LaunchMove);
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		Character->LaunchCharacter(
			Direction.GetSafeNormal() * Strength,
			true,
			true);
	}
}
