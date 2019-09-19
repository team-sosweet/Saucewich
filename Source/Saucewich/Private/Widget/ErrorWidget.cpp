// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/ErrorWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UErrorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	DescriptionText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Description")));
	BackgroundButton = Cast<UButton>(GetWidgetFromName(TEXT("Button_Background")));

	BackgroundButton->OnClicked.AddDynamic(this, &UErrorWidget::OnClicked);
}

void UErrorWidget::Activate(const FText& Message)
{
	DescriptionText->SetText(Message);
	AddToViewport(10);
}

void UErrorWidget::OnClicked()
{
	RemoveFromParent();
}
