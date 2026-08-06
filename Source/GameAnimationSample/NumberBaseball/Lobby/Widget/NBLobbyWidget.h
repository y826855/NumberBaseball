#pragma once

#include "Blueprint/UserWidget.h"
#include "NBLobbyWidget.generated.h"

class UEditableText;

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void JoinServer();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void ExitGame();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableText> ServerIPEditableText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableText> NicknameEditableText;
};
