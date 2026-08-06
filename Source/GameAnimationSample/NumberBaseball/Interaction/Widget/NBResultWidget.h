#pragma once

#include "Blueprint/UserWidget.h"
#include "GameAnimationSample/NumberBaseball/Struct/NBGuessResult.h"
#include "NBResultWidget.generated.h"

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowResult(ENBNumberMatchResult Result);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnResultShown(ENBNumberMatchResult Result);
};
