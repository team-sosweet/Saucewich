// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

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
	if (Found != INDEX_NONE) FocusedWidgets.RemoveAt(Found);

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
	const auto PC = GetOwningPlayerController();
	MenuWidget = CreateWidget<UBaseWidget>(PC, MenuWidgetClass.LoadSynchronous());
	ErrorWidget = CreateWidget<UErrorWidget>(PC, ErrorWidgetClass.LoadSynchronous());

	Super::BeginPlay();
}

UErrorWidget* ABaseHUD::ShowError(const FText Message)
{
	ErrorWidget->Activate(Message);
	return ErrorWidget;
}

void ABaseHUD::OpenMenu() const
{
	if (!MenuWidget->IsInViewport())
		MenuWidget->AddToViewport(2);
}
