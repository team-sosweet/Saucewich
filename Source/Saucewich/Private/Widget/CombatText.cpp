// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/CombatText.h"

#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"

#include "Player/TpsCharacter.h"

void UCombatText::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SizeBox = Cast<USizeBox>(GetWidgetFromName(TEXT("Box_Size")));
	DamageText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Damage")));
}

void UCombatText::ViewCombatText(const float Damage, ATpsCharacter* DamagedActor)
{
	const auto Location = DamagedActor->GetActorLocation();

	FVector2D ScreenPosition;
	if (!UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), Location, ScreenPosition))
		return;
	
	const auto OwnerLocation = GetOwningPlayerPawn()->GetActorLocation();
	const auto Distance = FVector::DistSquared(Location, OwnerLocation);

	const auto NormalizedDistance = UKismetMathLibrary::MapRangeClamped(Distance,
		MinDistance * MinDistance, MaxDistance * MaxDistance, 0.0f, 1.0f);

	const auto Size = NormalizedDistance *
		(SizeBox->MaxDesiredWidth - SizeBox->MinDesiredWidth) * -1.0f + SizeBox->MaxDesiredWidth;

	SizeBox->SetWidthOverride(Size);
	SizeBox->SetHeightOverride(Size);
	
	const auto SizeRatio = UKismetMathLibrary::MapRangeClamped(-Distance, 
		MaxDistance * MaxDistance * -1.0f, MinDistance * MinDistance * -1.0f, 0.0f, 1.0f);
	
	SetPositionInViewport(ScreenPosition +
		GetRandomPos(MinExtendSize * SizeRatio, MaxExtendSize * SizeRatio));

	const auto Green = UKismetMathLibrary::MapRangeClamped(Damage * -1.0f,
		MaxDamage * -1.0f, MinDamage * -1.0f, 0.0f, 1.0f);
	
	DamageText->SetColorAndOpacity(FLinearColor(1.0f, Green, 0.0f) * Color);

	const auto Text = UKismetTextLibrary::Conv_FloatToText(Damage,
		HalfToEven, false, true,
		1, 324, 0, 0);

	DamageText->SetText(Text);
	
	PlayAnimationForward(Fade);
	AddToViewport();
}

void UCombatText::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Fade)
	{
		OnRemove.ExecuteIfBound(this);
		RemoveFromParent();
	}
}

FVector2D UCombatText::GetRandomPos(const FVector2D& MinSize, const FVector2D& MaxSize)
{
	const auto Size = MaxSize - MinSize;
	
	auto RetX = FMath::RandRange(-Size.X, Size.X);
	auto RetY = FMath::RandRange(-Size.Y, Size.Y);

	RetX += MinSize.X * (RetX > 0.0f ? 1.0f : -1.0f);
	RetY += MinSize.Y * (RetY > 0.0f ? 1.0f : -1.0f);

	return FVector2D(RetX, RetY);
}
