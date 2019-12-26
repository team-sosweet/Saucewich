// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/BaseWidget.h"
#include <algorithm>
#include "GameFramework/InputSettings.h"
#include "Names.h"
#include "Player/BasePC.h"
#include "BaseHUD.h"
#include "Saucewich.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (bIsFocusable)
	{
		if (const auto PC = GetOwningPlayer())
		{
			CastChecked<ABasePC>(PC)->AddFocusedWidget(this);
			SetFocus();
		}
	}

	OnConstruct.Broadcast();
}

void UBaseWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (bIsFocusable)
	{
		if (const auto PC = GetOwningPlayer())
		{
			CastChecked<ABasePC>(PC)->RemoveFocusedWidget(this);
		}
	}

	OnDestruct.Broadcast();
}

FReply UBaseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsCloseable)
	{
		if (USaucewich::CheckInputAction(NAME("Close"), InKeyEvent))
		{
			RemoveFromParent();
		}
	}
	else if (USaucewich::CheckInputAction(NAME("Menu"), InKeyEvent))
	{
		CastChecked<ABaseHUD>(GetOwningPlayer()->GetHUD())->OpenMenu();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
