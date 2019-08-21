// Copyright 2019 Team Sosweet. All Rights Reserved.


#include "SelectModeWidget.h"
#include "WidgetTree.h"
#include "Components/Spacer.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void USelectModeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ModeWidgets.SetNumUninitialized(Modes.Num());

	ModeBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("Box_Modes")));

	FSlateChildSize ModeWidgetSize(ESlateSizeRule::Fill);
	FSlateChildSize SpacerSize(ESlateSizeRule::Fill);
	SpacerSize.Value = 0.1f;

	for (uint8 Idx = 0; Idx < Modes.Num(); Idx++)
	{
		UModeWidget* Widget = CreateModeWidget(Idx);
		Widget->OnClick.BindUObject(this, &USelectModeWidget::OnModeSelect);
		ModeBox->AddChildToHorizontalBox(Widget)->SetSize(ModeWidgetSize);
		ModeWidgets[Idx] = Widget;

		if (Idx + 1 < Modes.Num())
		{
			UWidget* Spacer = WidgetTree->ConstructWidget<USpacer>();
			ModeBox->AddChildToHorizontalBox(Spacer)->SetSize(SpacerSize);
		}
	}

	OnModeSelect(0);
}

void USelectModeWidget::OnModeSelect(uint8 Index)
{
	SelectIndex = Index;

	for (uint8 Idx = 0; Idx < Modes.Num(); Idx++)
	{
		ModeWidgets[Idx]->SetSelect(Idx == Index);
	}
}