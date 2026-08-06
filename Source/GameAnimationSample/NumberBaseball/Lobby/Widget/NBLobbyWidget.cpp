#include "NBLobbyWidget.h"

#include "Components/EditableText.h"
#include "GameAnimationSample/NumberBaseball/Lobby/NBLobbyPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

void UNBLobbyWidget::JoinServer()
{
	if (IsValid(ServerIPEditableText) == false || IsValid(NicknameEditableText) == false)
	{
		return;
	}

	ANBLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<ANBLobbyPlayerController>();
	if (IsValid(LobbyPlayerController))
	{
		FString Nickname = NicknameEditableText->GetText().ToString().TrimStartAndEnd();
		if (Nickname.IsEmpty() || Nickname == "Player")
		{
			Nickname = FString::Printf(TEXT("Player%d"), FMath::RandRange(1000, 9999));
		}

		LobbyPlayerController->JoinServer(
			ServerIPEditableText->GetText().ToString().TrimStartAndEnd(),
			Nickname);
	}
}

void UNBLobbyWidget::ExitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
