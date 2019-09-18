// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/ErrorWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

bool UErrorWidget::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Oh"));
	return Super::Initialize();
}

void UErrorWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UE_LOG(LogTemp, Warning, TEXT("Please"));
	DescriptionText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Description")));
	BackgroundButton = Cast<UButton>(GetWidgetFromName(TEXT("Button_Background")));

	BackgroundButton->OnClicked.AddDynamic(this, &UErrorWidget::OnClicked);
}

void UErrorWidget::Activate(const FText& Message)
{
	DescriptionText->SetText(Message);
	AddToViewport();
}

void UErrorWidget::OnClicked()
{
	RemoveFromParent();
}
