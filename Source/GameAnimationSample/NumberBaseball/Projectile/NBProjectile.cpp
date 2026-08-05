#include "NBProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameAnimationSample/NumberBaseball/AbilitySystem/Effects/NBKnockbackEffect.h"
#include "GameAnimationSample/NumberBaseball/Character/Components/NBKnockbackComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"

ANBProjectile::ANBProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	InitialLifeSpan = 5.f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->InitSphereRadius(15.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOverlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->MaxSpeed = 2000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	ImpactEffectClass = UNBKnockbackEffect::StaticClass();
}

void ANBProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);
	}

	if (APawn* InstigatorPawn = GetInstigator())
	{
		CollisionComponent->IgnoreActorWhenMoving(InstigatorPawn, true);
	}
}

void ANBProjectile::HandleOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (HasAuthority() == false || bOverlapProcessed ||
		IsValid(OtherActor) == false || OtherActor == GetOwner())
	{
		return;
	}

	bOverlapProcessed = true;
	UAbilitySystemComponent* SourceAbilitySystem =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	UAbilitySystemComponent* TargetAbilitySystem =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	if (IsValid(SourceAbilitySystem) &&
		IsValid(TargetAbilitySystem) &&
		ImpactEffectClass != nullptr)
	{
		FGameplayEffectContextHandle EffectContext = SourceAbilitySystem->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		SourceAbilitySystem->ApplyGameplayEffectToTarget(
			ImpactEffectClass->GetDefaultObject<UGameplayEffect>(),
			TargetAbilitySystem,
			1.f,
			EffectContext);
	}

	if (UNBKnockbackComponent* KnockbackComponent =
		OtherActor->FindComponentByClass<UNBKnockbackComponent>())
	{
		KnockbackComponent->ApplyKnockback(
			ProjectileMovementComponent->Velocity,
			KnockbackStrength);
	}

	MulticastOverlap(GetActorLocation());
	SetLifeSpan(0.1f);
}

void ANBProjectile::MulticastOverlap_Implementation(const FVector& ImpactLocation)
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent->StopMovementImmediately();
	OnProjectileOverlap(ImpactLocation);
}
