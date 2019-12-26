// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/BasePC.h"
#include "Components/Widget.h"

void ABasePC::AddFocusedWidget(UWidget* const Widget)
{
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Widget->GetCachedWidget());
	SetInputMode(InputMode);

	FocusedWidgets.Add(Widget);
}

void ABasePC::RemoveFocusedWidget(UWidget* const Widget)
{
	FocusedWidgets.Remove(Widget);
	
	if (FocusedWidgets.Num() == 0)
	{
		SetInputMode(FInputModeGameOnly{});
	}
}
