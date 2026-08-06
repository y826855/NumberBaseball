#include "NBResultWidget.h"

void UNBResultWidget::ShowResult(ENBNumberMatchResult Result)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	OnResultShown(Result);
}
