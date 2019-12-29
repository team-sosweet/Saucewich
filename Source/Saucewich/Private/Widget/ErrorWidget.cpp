// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/ErrorWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Names.h"

UErrorWidget::UErrorWidget()
{
	bIsCloseable = true;
	bIsFocusable = true;
}

void UErrorWidget::Activate(FText&& Message)
{
	Text->SetText(MoveTemp(Message));
	AddToViewport(10);
}

void UErrorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Text = CastChecked<UTextBlock>(GetWidgetFromName(NAME("Text_Description")));
}
