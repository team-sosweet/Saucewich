// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/ErrorWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Names.h"

UErrorWidget::UErrorWidget()
{
	bIsCloseable = true;
	bIsFocusable = true;
}

void UErrorWidget::Activate(const FText& Message)
{
	Text->SetText(Message);
	if (!IsInViewport()) AddToViewport(10);
}

void UErrorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Text = CastChecked<UTextBlock>(GetWidgetFromName(NAME("Text_Description")));
}
