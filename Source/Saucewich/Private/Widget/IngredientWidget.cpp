// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Widget/IngredientWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridSlot.h"

void UIngredientWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	IngredientsNum = IngredientPanels.Num();	
	IngredientsSlot.Init(nullptr, IngredientsNum);
	IngredientContentClasses.Init(nullptr, IngredientsNum);
	IngredientContentSlots.Init(nullptr, IngredientsNum);
	IsIngredientActives.Init(false, IngredientsNum);
	InactiveVisibility = IsCentered ? ESlateVisibility::Collapsed : ESlateVisibility::Hidden;
	
	for (auto Index = 0; Index < IngredientsNum; Index++)
	{
		IngredientsSlot[Index] = Cast<UUniformGridSlot>(IngredientPanels[Index]->Slot);
		IngredientPanels[Index]->SetVisibility(InactiveVisibility);
		
		const auto IngredientContent = IngredientPanels[Index]->GetChildAt(0);
		IngredientContentClasses[Index] = IngredientContent->GetClass();
		IngredientContentSlots[Index] = Cast<UCanvasPanelSlot>(IngredientContent->Slot);
		check(IngredientContent && IngredientContentClasses[Index]);
	}
}

void UIngredientWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const auto IngredientNums = GetIngredientNums();
	auto InactiveRow = IngredientsNum;
	
	for (auto Index = 0; Index < IngredientsNum; Index++)
	{
		if (IngredientNums[Index] > 0)
		{
			SetIngredientImage(Index, IngredientNums[Index]);

			if (!IsIngredientActives[Index])
			{
				IngredientPanels[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

				if (IsVertical)
				{
					IngredientsSlot[Index]->SetColumn(ActiveNum++);
				}
				else
				{
					IngredientsSlot[Index]->SetRow(ActiveNum++);
				}
				
				IsIngredientActives[Index] = true;
			}
		}
		else
		{
			if (IsVertical)
			{
				IngredientsSlot[Index]->SetColumn(--InactiveRow);
			}
			else
			{
				IngredientsSlot[Index]->SetRow(--InactiveRow);
			}
			
			
			if (IsIngredientActives[Index])
			{
				IngredientPanels[Index]->SetVisibility(InactiveVisibility);
				IsIngredientActives[Index] = false;
				ActiveNum--;
			}
		}
	}

	OnSetVisibility(ActiveNum);
}

void UIngredientWidget::SetIngredientImage(const uint8 IngredientIndex, const uint8 Num)
{
	const auto& IngredientPanel = IngredientPanels[IngredientIndex];

	for (auto Needs = Num - IngredientPanel->GetChildrenCount(); Needs > 0; Needs--)
	{
		const auto NewWidget = CreateWidget(GetOwningPlayer(), IngredientContentClasses[IngredientIndex]);
		const auto NewSlot = IngredientPanel->AddChildToCanvas(NewWidget);
		const auto SlotData = IngredientContentSlots[IngredientIndex];
		
		NewSlot->SetAnchors(SlotData->GetAnchors());
		NewSlot->SetSize(SlotData->GetSize());
		NewSlot->SetAlignment(SlotData->GetAlignment());
		NewSlot->SetAutoSize(SlotData->GetAutoSize());

		const auto Distance = GetDistanceFromIndex(IngredientPanel->GetChildrenCount());
		const auto Pos = SlotData->GetPosition() + Distance;
		NewSlot->SetPosition(Pos);
	}

	const auto Ingredients = IngredientPanel->GetAllChildren();
	const auto IngredientNum = Ingredients.Num();
	
	for (auto Index = 0; Index < IngredientNum; Index++)
	{
		const auto WidgetSlot = Cast<UCanvasPanelSlot>(Ingredients[Index]->Slot);
		WidgetSlot->SetZOrder(IngredientNum - Index);
		
		const auto IngredientVisibility = Index < Num ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
		Ingredients[Index]->SetVisibility(IngredientVisibility);
	}
}