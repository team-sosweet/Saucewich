// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "BaseHUD.h"

#include "Widget/ErrorWidget.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	ErrorWidget = CreateWidget<UErrorWidget>(GetOwningPlayerController(), ErrorWidgetClass);
}

void ABaseHUD::ShowError(const FText Message)
{
	ErrorWidget->Activate(Message);
}
