// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/ScaleProgressBar.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UScaleProgressBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	LeftImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_Left")));
	RightImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_Right")));
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_Mid")));

	RightSlot = Cast<UCanvasPanelSlot>(RightImage->Slot);
	StartAnchor = RightSlot->GetAnchors();

	const auto ProgressAnchor = Cast<UCanvasPanelSlot>(ProgressBar->Slot)->GetAnchors();
	ProgressMinAnchor = ProgressAnchor.Minimum.X;
	ProgressMaxAnchor = ProgressAnchor.Maximum.X;
}

void UScaleProgressBar::SetValue(const float InValue)
{
	Value = InValue;
	ProgressBar->SetPercent(Value);

	const auto NewAnchor = GetAnchorByValue();
	RightSlot->SetAnchors(NewAnchor);
}

FAnchors UScaleProgressBar::GetAnchorByValue()
{
	const auto Pos = Value * (ProgressMaxAnchor - ProgressMinAnchor) + ProgressMinAnchor;
	const auto SlotDistance = StartAnchor.Maximum.X - StartAnchor.Minimum.X;

	return FAnchors{ Pos, StartAnchor.Minimum.Y, Pos + SlotDistance, StartAnchor.Maximum.Y };
}
