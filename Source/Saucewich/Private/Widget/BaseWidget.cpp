// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/BaseWidget.h"
#include <algorithm>
#include "GameFramework/InputSettings.h"
#include "Names.h"
#include "Player/BasePC.h"

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
		const auto InputSettings = GetDefault<UInputSettings>();
		TArray<FInputActionKeyMapping> Mappings;
		InputSettings->GetActionMappingByName(NAME("Close"), Mappings);
		
		const auto bClose = std::any_of(Mappings.begin(), Mappings.end(), [&](const FInputActionKeyMapping& Mapping)
		{
			return Mapping.Key == InKeyEvent.GetKey();
		});

		if (bClose)
		{
			RemoveFromParent();
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
