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
	UE_DEFINE_GAMEPLAY_TAG(TimerChanged, "NumberBaseball.Timer.Changed");
	UE_DEFINE_GAMEPLAY_TAG(NotifyReceived, "NumberBaseball.Notify.Received");
	UE_DEFINE_GAMEPLAY_TAG(TurnPlayerChanged, "NumberBaseball.Turn.PlayerChanged");
	UE_DEFINE_GAMEPLAY_TAG(ScoreChanged, "NumberBaseball.Score.Changed");
}

namespace NBTimerTypes
{
	UE_DEFINE_GAMEPLAY_TAG(GameStart, "NumberBaseball.Timer.GameStart");
	UE_DEFINE_GAMEPLAY_TAG(UserInput, "NumberBaseball.Timer.UserInput");
}

namespace NBNotifyTypes
{
	UE_DEFINE_GAMEPLAY_TAG(WaitingForPlayers, "NumberBaseball.Notify.WaitingForPlayers");
	UE_DEFINE_GAMEPLAY_TAG(WaitingForGameStart, "NumberBaseball.Notify.WaitingForGameStart");
	UE_DEFINE_GAMEPLAY_TAG(YourTurn, "NumberBaseball.Notify.YourTurn");
	UE_DEFINE_GAMEPLAY_TAG(OpponentTurn, "NumberBaseball.Notify.OpponentTurn");
}
