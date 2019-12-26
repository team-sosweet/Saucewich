// Copyright 2019 Othereum. All Rights Reserved.

#include "BaseHUD.h"
#include "Widget/ErrorWidget.h"

void ABaseHUD::BeginPlay()
{
	ErrorWidget = CreateWidget<UErrorWidget>(GetOwningPlayerController(), ErrorWidgetClass.LoadSynchronous());
	MenuWidget = CreateWidget<UBaseWidget>(GetOwningPlayerController(), MenuWidgetClass.LoadSynchronous());

	Super::BeginPlay();
}

void ABaseHUD::ShowError(const FText Message, const bool bCritical)
{
	ErrorWidget->Activate(Message, bCritical);
	OnShowError();
}

void ABaseHUD::OpenMenu() const
{
	if (!MenuWidget->IsInViewport())
		MenuWidget->AddToViewport(2);
}
