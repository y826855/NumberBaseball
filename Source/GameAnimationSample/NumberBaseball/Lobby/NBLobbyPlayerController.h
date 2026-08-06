#pragma once

#include "GameFramework/PlayerController.h"
#include "NBLobbyPlayerController.generated.h"

class UUserWidget;

UCLASS()
class GAMEANIMATIONSAMPLE_API ANBLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void JoinServer(const FString& ServerAddress, const FString& Nickname);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby|UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> LobbyWidget;
};
