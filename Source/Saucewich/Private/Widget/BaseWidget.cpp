// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/BaseWidget.h"
#include "GameFramework/InputSettings.h"
#include "Names.h"
#include "Player/BasePC.h"
#include "Player/BaseHUD.h"
#include "Saucewich.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (bIsFocusable)
	{
		if (const auto PC = GetOwningPlayer())
		{
			CastChecked<ABaseHUD>(PC->GetHUD())->AddFocusedWidget(this);
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
			CastChecked<ABaseHUD>(PC->GetHUD())->RemoveFocusedWidget(this);
		}
	}

	OnDestruct.Broadcast();
	OnDestructNativeSingle.ExecuteIfBound();
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
