#include "NBLobbyPlayerController.h"

#include "Blueprint/UserWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogNBLobby, Log, All);

void ANBLobbyPlayerController::JoinServer(const FString& ServerAddress, const FString& Nickname)
{
	if (ServerAddress.IsEmpty())
	{
		return;
	}

	FString IPAddress;
	FString Port;
	if (ServerAddress.Split(TEXT(":"), &IPAddress, &Port, ESearchCase::IgnoreCase, ESearchDir::FromEnd) == false)
	{
		IPAddress = ServerAddress;
		Port = TEXT("Default");
	}

	UE_LOG(LogNBLobby, Display, TEXT("Join server requested: IP=%s Port=%s"), *IPAddress, *Port);

	const FString TravelURL = FString::Printf(TEXT("%s?Name=%s"), *ServerAddress, *Nickname);
	ClientTravel(TravelURL, TRAVEL_Absolute);
}

void ANBLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false || LobbyWidgetClass == nullptr)
	{
		return;
	}

	LobbyWidget = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
	if (IsValid(LobbyWidget) == false)
	{
		return;
	}

	LobbyWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}
