// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/SelectModeWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "WidgetTree.h"

void USelectModeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ModeWidgets.SetNum(Modes.Num());

	ModeBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("Box_Modes")));

	FSlateChildSize SpacerSize(ESlateSizeRule::Fill);
	SpacerSize.Value = 0.1f;

	for (uint8 Idx = 0; Idx < Modes.Num(); Idx++)
	{
		const auto Widget = CreateModeWidget(Idx);
		Widget->OnClick.BindUObject(this, &USelectModeWidget::OnModeSelect);
		ModeBox->AddChildToHorizontalBox(Widget)->SetSize(ESlateSizeRule::Fill);
		ModeWidgets[Idx] = Widget;

		if (Idx + 1 < Modes.Num())
		{
			const auto Spacer = WidgetTree->ConstructWidget<USpacer>();
			ModeBox->AddChildToHorizontalBox(Spacer)->SetSize(SpacerSize);
		}
	}

	OnModeSelect(0);
}

void USelectModeWidget::OnModeSelect(const uint8 Index)
{
	SelectIndex = Index;

	for (uint8 Idx = 0; Idx < Modes.Num(); Idx++)
	{
		ModeWidgets[Idx]->SetSelect(Idx == Index);
	}
}