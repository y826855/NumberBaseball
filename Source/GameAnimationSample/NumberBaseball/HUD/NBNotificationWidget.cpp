#include "NBNotificationWidget.h"

#include "Components/TextBlock.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/Components/NBClientMessageComponent.h"
#include "GameAnimationSample/NumberBaseball/PlayerController/NBPlayerController.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGameplayMessages.h"

void UNBNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);

	NotifyListenerHandle = UGameplayMessageSubsystem::Get(this)
		.RegisterListener<FNBNotifyMessage>(
			NBGameplayMessages::NotifyReceived,
			this,
			&ThisClass::HandleNotifyReceived);

	const ANBPlayerController* PlayerController = Cast<ANBPlayerController>(GetOwningPlayer());
	const UNBClientMessageComponent* ClientMessageComponent = IsValid(PlayerController)
		? PlayerController->GetClientMessageComponent()
		: nullptr;
	FNBNotifyMessage LatestMessage;
	if (IsValid(ClientMessageComponent)
		&& ClientMessageComponent->GetLatestNotifyMessage(LatestMessage))
	{
		HandleNotifyReceived(NBGameplayMessages::NotifyReceived, LatestMessage);
	}
}

void UNBNotificationWidget::NativeDestruct()
{
	NotifyListenerHandle.Unregister();
	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);
	Super::NativeDestruct();
}

void UNBNotificationWidget::HandleNotifyReceived(
	FGameplayTag Channel,
	const FNBNotifyMessage& Message)
{
	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);

	if (Message.bIsActive == false)
	{
		HideNotification();
		return;
	}

	MessageText->SetText(Message.Message);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	OnNotificationReceived(Message);

	if (Message.DisplayDuration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			HideTimerHandle,
			this,
			&ThisClass::HideNotification,
			Message.DisplayDuration,
			false);
	}
}

void UNBNotificationWidget::HideNotification()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
