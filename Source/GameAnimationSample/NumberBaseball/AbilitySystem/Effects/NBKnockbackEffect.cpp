#include "NBKnockbackEffect.h"

UNBKnockbackEffect::UNBKnockbackEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.f));
}
