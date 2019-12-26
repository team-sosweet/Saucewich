// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/BasePC.h"
#include "Components/Widget.h"

void ABasePC::AddFocusedWidget(UWidget* const Widget)
{
	static const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	FocusedWidgets.Add(Widget);
}

void ABasePC::RemoveFocusedWidget(UWidget* const Widget)
{
	const auto Found = FocusedWidgets.FindLast(Widget);
	if (ensure(Found != INDEX_NONE)) FocusedWidgets.RemoveAt(Found);
	
	if (FocusedWidgets.Num() == 0)
	{
		static const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
	else
	{
		FocusedWidgets.Last()->SetFocus();
	}
}
