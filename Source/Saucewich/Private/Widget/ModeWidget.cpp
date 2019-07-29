// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ModeWidget.h"
#include "Components/Button.h"

void UModeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ModeButton = Cast<UButton>(GetWidgetFromName(TEXT("Button")));
	check(ModeButton != nullptr);

	ModeButton->OnClicked.AddDynamic(this, &UModeWidget::OnClicked);
}

void UModeWidget::SetSelect(bool bIsSelect)
{
	bIsSelected = bIsSelect;
}

void UModeWidget::OnClicked()
{
	OnClick.ExecuteIfBound(Index);
}