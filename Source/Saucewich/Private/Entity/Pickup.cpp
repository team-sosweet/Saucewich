// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Pickup.h"
#include "Components/StaticMeshComponent.h"

APickup::APickup()
	:SceneRoot{CreateDefaultSubobject<USceneComponent>("SceneRoot")},
	Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")}
{
	RootComponent = SceneRoot;
	Mesh->SetupAttachment(SceneRoot);
	PrimaryActorTick.bCanEverTick = true;
}

void APickup::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const auto Time = GetGameTimeSinceCreation();
	Mesh->RelativeLocation.Z = FMath::Sin(Time * BounceSpeed) * BounceScale;
	Mesh->RelativeRotation.Yaw += DeltaSeconds * RotateSpeed;
	Mesh->UpdateComponentToWorld();
}
