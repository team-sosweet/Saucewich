// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/BaseHUD.h"
#include "Widget/ErrorWidget.h"

void ABaseHUD::AddFocusedWidget(UWidget* const Widget)
{
	static const FInputModeUIOnly InputMode;
	GetOwningPlayerController()->SetInputMode(InputMode);
	FocusedWidgets.Add(Widget);
}

void ABaseHUD::RemoveFocusedWidget(UWidget* const Widget)
{
	const auto Found = FocusedWidgets.FindLast(Widget);
	if (ensure(Found != INDEX_NONE)) FocusedWidgets.RemoveAt(Found);

	if (FocusedWidgets.Num() == 0)
	{
		static const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
	}
	else
	{
		do
		{
			if (const auto TopWidget = FocusedWidgets.Last().Get())
			{
				TopWidget->SetFocus();
				break;
			}
			FocusedWidgets.Pop();
		}
		while (FocusedWidgets.Num() > 0);
	}
}

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
