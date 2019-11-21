// Copyright 2019 Othereum. All Rights Reserved.

#include "BaseHUD.h"

#include "Widget/ErrorWidget.h"

#include "Saucewich.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	ErrorWidget = CreateWidget<UErrorWidget>(GetOwningPlayerController(), ErrorWidgetClass);
}

void ABaseHUD::ShowError(const FText Message, const bool bCritical)
{
	if (ErrorWidget)
	{
		ErrorWidget->Activate(Message, bCritical);
		OnShowError();
	}
	else
	{
		UE_LOG(LogSaucewich, Error, TEXT("ABaseHUD::ShowError called before BeginPlay. It'll be ignored."));
	}
}
