#pragma once

#include "Components/ActorComponent.h"
#include "NBKnockbackComponent.generated.h"

UCLASS(ClassGroup = (NumberBaseball), meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSAMPLE_API UNBKnockbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNBKnockbackComponent();

	UFUNCTION(BlueprintCallable, Category = "Knockback")
	void ApplyKnockback(
		const FVector& Direction,
		float Strength);
};
