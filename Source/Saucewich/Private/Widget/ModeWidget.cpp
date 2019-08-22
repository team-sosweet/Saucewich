// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/ModeWidget.h"
#include "Components/Button.h"

void UModeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ModeButton = Cast<UButton>(GetWidgetFromName(TEXT("Button")));
	ModeButton->OnClicked.AddDynamic(this, &UModeWidget::OnClicked);
}

void UModeWidget::SetSelect(const bool bIsSelect)
{
	bIsSelected = bIsSelect;
}

void UModeWidget::OnClicked()
{
	OnClick.ExecuteIfBound(Index);
}