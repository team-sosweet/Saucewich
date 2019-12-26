// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/BasePC.h"
#include "Components/Widget.h"
#include "Names.h"
#include "BaseHUD.h"

void ABasePC::AddFocusedWidget(UWidget* const Widget)
{
#if !UE_SERVER
	static const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	FocusedWidgets.Add(Widget);
#endif 
}

void ABasePC::RemoveFocusedWidget(UWidget* const Widget)
{
#if !UE_SERVER
	const auto Found = FocusedWidgets.FindLast(Widget);
	if (ensure(Found != INDEX_NONE)) FocusedWidgets.RemoveAt(Found);

	if (FocusedWidgets.Num() == 0)
	{
		static const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
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
#endif 
}

void ABasePC::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(NAME("Menu"), IE_Pressed, this, &ABasePC::OpenMenu);
}

void ABasePC::OpenMenu()
{
	CastChecked<ABaseHUD>(GetHUD())->OpenMenu();
}
