#pragma once

#include "Blueprint/UserWidget.h"
#include "NBNumberWidget.generated.h"

UCLASS(Abstract)
class GAMEANIMATIONSAMPLE_API UNBNumberWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetNumber(int32 Number);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "NumberPad")
	void OnNumberSet(int32 Number);
};
