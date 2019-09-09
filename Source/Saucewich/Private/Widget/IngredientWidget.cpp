// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/IngredientWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridSlot.h"

void UIngredientWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	IngredientsNum = IngredientPanels.Num();
	IngredientsSlot.Init(nullptr, IngredientsNum);
	IsIngredientActives.Init(false, IngredientsNum);

	for (auto Index = 0; Index < IngredientsNum; Index++)
	{
		IngredientsSlot[Index] = Cast<UUniformGridSlot>(IngredientPanels[Index]->Slot);
	}
}

void UIngredientWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const auto IngredientNums = GetIngredientNums();

	for (auto Index = 0; Index < IngredientsNum; Index++)
	{
		if (IsIngredientActives[Index] && IngredientNums[Index] == 0)
		{
			IngredientPanels[Index]->SetVisibility(ESlateVisibility::Hidden);
			IsIngredientActives[Index] = false;
			ActiveNum--;
		}
	}

	for (auto Index = 0; Index < IngredientsNum; Index++)
	{
		if (IngredientNums[Index] > 0)
		{
			SetIngredientImage(Index, IngredientNums[Index]);

			if (!IsIngredientActives[Index])
			{
				IngredientPanels[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				IngredientsSlot[Index]->SetRow(ActiveNum++);
				IsIngredientActives[Index] = true;
			}
		}
	}
}

void UIngredientWidget::SetIngredientImage(const uint8 IngredientIndex, const uint8 Num)
{
	const auto& IngredientPanel = IngredientPanels[IngredientIndex];

	for (auto Needs = Num - IngredientPanel->GetChildrenCount(); Needs > 0; Needs--)
	{
		const auto NewWidget = CreateWidget(GetOwningPlayer(), IngredientContentClasses[IngredientIndex]);
		const auto NewSlot = IngredientPanels[IngredientIndex]->AddChildToCanvas(NewWidget);
		const auto SlotData = IngredientContentSlots[IngredientIndex];
		
		NewSlot->SetAnchors(SlotData->GetAnchors());
		NewSlot->SetPosition(SlotData->GetPosition());
		NewSlot->SetSize(SlotData->GetSize());
		NewSlot->SetAlignment(SlotData->GetAlignment());
		NewSlot->SetAutoSize(SlotData->GetAutoSize());
		NewSlot->SetZOrder(SlotData->GetZOrder());
	}

	const auto Ingredients = IngredientPanel->GetAllChildren();
	
	for (auto Index = 0; Index < Ingredients.Num(); Index++)
	{
		const auto IngredientVisibility = Index < Num ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
		Ingredients[Index]->SetVisibility(IngredientVisibility);
	}
}