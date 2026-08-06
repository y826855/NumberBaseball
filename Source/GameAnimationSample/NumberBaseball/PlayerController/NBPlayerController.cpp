#include "NBPlayerController.h"

#include "GameAnimationSample/NumberBaseball/Cheat/NBCheatManager.h"
#include "GameAnimationSample/NumberBaseball/Interaction/Widget/NBNumberPadWidget.h"
#include "GameAnimationSample/NumberBaseball/HUD/NBNotificationWidget.h"
#include "GameAnimationSample/NumberBaseball/HUD/NBRoundScoreWidget.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBClientMessageComponent.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBPendingTaskClientComponent.h"

ANBPlayerController::ANBPlayerController()
{
	CheatClass = UNBCheatManager::StaticClass();

	PendingTaskClientComponent = CreateDefaultSubobject<UNBPendingTaskClientComponent>(
		TEXT("PendingTaskClientComponent"));
	ClientMessageComponent = CreateDefaultSubobject<UNBClientMessageComponent>(
		TEXT("ClientMessageComponent"));
}

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();
	EnableCheats();

	if (IsLocalController() == false)
	{
		return;
	}

	if (NumberPadWidgetClass != nullptr)
	{
		NumberPadWidget = CreateWidget<UNBNumberPadWidget>(this, NumberPadWidgetClass);
		if (IsValid(NumberPadWidget))
		{
			NumberPadWidget->AddToViewport();
		}
	}

	if (NotificationWidgetClass != nullptr)
	{
		NotificationWidget = CreateWidget<UNBNotificationWidget>(this, NotificationWidgetClass);
		if (IsValid(NotificationWidget))
		{
			NotificationWidget->AddToViewport();
		}
	}

	if (RoundScoreWidgetClass != nullptr)
	{
		RoundScoreWidget = CreateWidget<UNBRoundScoreWidget>(this, RoundScoreWidgetClass);
		if (IsValid(RoundScoreWidget))
		{
			RoundScoreWidget->AddToViewport();
		}
	}
}
