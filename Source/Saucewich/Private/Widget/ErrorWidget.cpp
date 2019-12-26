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

void UErrorWidget::Activate(const FText& Message, const bool bCritical)
{
	bCriticalError = bCritical;
	DescriptionText->SetText(Message);
	AddToViewport(10);
}

void UErrorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	DescriptionText = CastChecked<UTextBlock>(GetWidgetFromName(NAME("Text_Description")));
	BackgroundButton = CastChecked<UButton>(GetWidgetFromName(NAME("Button_Background")));

	BackgroundButton->OnClicked.AddDynamic(this, &UErrorWidget::OnClicked);
}

void UErrorWidget::OnClicked()
{
	if (bCriticalError) FPlatformMisc::RequestExit(false);
	RemoveFromParent();
}
