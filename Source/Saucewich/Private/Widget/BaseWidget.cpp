// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/BaseWidget.h"
#include <algorithm>
#include "GameFramework/InputSettings.h"
#include "Names.h"
#include "Player/BasePC.h"
#include "BaseHUD.h"

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
	const auto PressedKey = InKeyEvent.GetKey();
	const auto InputSettings = GetDefault<UInputSettings>();
	const auto IsInputAction = [&](const FName ActionName)
	{
		TArray<FInputActionKeyMapping> Mappings;
		InputSettings->GetActionMappingByName(ActionName, Mappings);
		return std::any_of(Mappings.begin(), Mappings.end(), [&](const FInputActionKeyMapping& Mapping)
		{
			return Mapping.Key == PressedKey;
		});
	};

	if (bIsCloseable)
	{
		if (IsInputAction(NAME("Close")))
		{
			RemoveFromParent();
		}
	}
	else if (IsInputAction(NAME("Menu")))
	{
		CastChecked<ABaseHUD>(GetOwningPlayer()->GetHUD())->OpenMenu();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
