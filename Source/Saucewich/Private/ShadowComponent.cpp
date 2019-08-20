// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ShadowComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UShadowComponent::UShadowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BodyInstance.SetCollisionProfileNameDeferred("NoCollision");
}

void UShadowComponent::BeginPlay()
{
	Super::BeginPlay();
	Offset.SetLocation(RelativeLocation);
	Material = CreateDynamicMaterialInstance(0);
}

void UShadowComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* const ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto* const Owner = GetOwner();
	const auto StartTransform = Offset * Owner->GetRootComponent()->GetComponentTransform();
	const auto Start = StartTransform.GetLocation();
	auto End = Start;
	End.Z -= MaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	FHitResult Hit;
	const auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	SetVisibility(bHit);
	if (bHit)
	{
		Hit.Location.Z += .01f;
		SetWorldLocationAndRotation(
			Hit.Location,
			Hit.Normal.RotateAngleAxis(90.f, FVector::RightVector).Rotation()
		);
		auto Dark = Darkness;
		if (bTranslucent) Dark /= 2;
		Material->SetScalarParameterValue("Darkness", (1.f - (Start.Z - Hit.Location.Z) / MaxDistance) * Dark);
	}
}
