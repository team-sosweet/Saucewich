// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/BaseWidget.h"
#include <algorithm>
#include "GameFramework/InputSettings.h"
#include "Names.h"

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBaseWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UBaseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsFocusable)
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
