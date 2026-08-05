#include "NBGameplayMessages.h"

namespace NBGameplayMessages
{
	UE_DEFINE_GAMEPLAY_TAG(InputSubmitted, "NumberBaseball.Input.Submitted");
	UE_DEFINE_GAMEPLAY_TAG(GamePhaseChanged, "NumberBaseball.GamePhase.Changed");
	UE_DEFINE_GAMEPLAY_TAG(TurnPhaseChanged, "NumberBaseball.TurnPhase.Changed");
	UE_DEFINE_GAMEPLAY_TAG(InputValuesChanged, "NumberBaseball.InputValues.Changed");
	UE_DEFINE_GAMEPLAY_TAG(RoundStateChanged, "NumberBaseball.RoundState.Changed");
	UE_DEFINE_GAMEPLAY_TAG(GuessResultChanged, "NumberBaseball.GuessResult.Changed");
	UE_DEFINE_GAMEPLAY_TAG(PendingTaskChanged, "NumberBaseball.PendingTask.Changed");
}
