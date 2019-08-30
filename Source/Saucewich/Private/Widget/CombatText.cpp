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
	
	SetPositionInViewport(ScreenPosition + GetRandomPos());

	const auto OwnerLocation = GetOwningPlayerPawn()->GetActorLocation();
	const auto Distance = FVector::DistSquared(Location, OwnerLocation);

	const auto NormalizedDistance = UKismetMathLibrary::MapRangeClamped(Distance,
		MinDistance * MinDistance, MaxDistance * MaxDistance, 0.0f, 1.0f);

	const auto Size = NormalizedDistance *
		(SizeBox->MaxDesiredWidth - SizeBox->MinDesiredWidth) * -1.0f + SizeBox->MaxDesiredWidth;

	SizeBox->SetWidthOverride(Size);
	SizeBox->SetHeightOverride(Size);

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
		OnRemove.Broadcast(this);
		RemoveFromParent();
	}
}

FVector2D UCombatText::GetRandomPos()
{
	const auto ExtendSize = MaxExtendSize - MinExtendSize;
	
	auto RetX = FMath::RandRange(-ExtendSize.X, ExtendSize.X);
	auto RetY = FMath::RandRange(-ExtendSize.Y, ExtendSize.Y);

	RetX += MinExtendSize.X * (RetX > 0.0f ? 1.0f : -1.0f);
	RetY += MinExtendSize.Y * (RetY > 0.0f ? 1.0f : -1.0f);

	return FVector2D(RetX, RetY);
}
