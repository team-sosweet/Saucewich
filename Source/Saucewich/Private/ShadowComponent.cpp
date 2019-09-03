// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ShadowComponent.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

UShadowComponent::UShadowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BodyInstance.SetCollisionProfileNameDeferred("NoCollision");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane{TEXT("/Engine/BasicShapes/Plane")};
	Super::SetStaticMesh(Plane.Object);
	bVisible = false;
}

float UShadowComponent::GetMaxDist() const
{
	return RelativeScale3D.X * 200;
}

void UShadowComponent::BeginPlay()
{
	Super::BeginPlay();
	Offset.SetLocation(RelativeLocation);
	CreateDynamicMaterialInstance(0);
}

void UShadowComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* const ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto* const Owner = GetOwner();
	const auto StartTransform = Offset * GetAttachParent()->GetComponentTransform();
	const auto Start = StartTransform.GetLocation();
	auto End = Start;
	End.Z -= GetMaxDist();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	FHitResult Hit;
	const auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	SetVisibility(bHit);
	if (bHit)
	{
		Hit.Location.Z += .1f;
		SetWorldLocationAndRotation(
			Hit.Location,
			Hit.Normal.RotateAngleAxis(90.f, FVector::RightVector).Rotation()
		);

		auto Dist = (Start.Z - Hit.Location.Z) / GetMaxDist();
		if (bTranslucent) Dist = (Dist + 1) / 2;
		static_cast<UMaterialInstanceDynamic*>(GetMaterial(0))->SetScalarParameterValue("Distance", Dist);
	}
}
