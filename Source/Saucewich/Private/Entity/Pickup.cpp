// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "ShadowComponent.h"

APickup::APickup()
	:Collision{CreateDefaultSubobject<USphereComponent>("Collision")},
	Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")},
	Shadow{CreateDefaultSubobject<UShadowComponent>("Shadow")}
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
	
	RootComponent = Collision;
	Collision->BodyInstance.SetCollisionProfileNameDeferred("Pickup");
	
	Mesh->SetupAttachment(Collision);
	Mesh->BodyInstance.SetCollisionProfileNameDeferred("NoCollision");
	
	Shadow->SetupAttachment(Mesh);
	Shadow->RelativeScale3D = FVector{Collision->GetScaledSphereRadius() / 50};
}

void APickup::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const auto Time = GetGameTimeSinceCreation();
	Mesh->RelativeLocation.Z = FMath::Sin(Time * BounceSpeed) * BounceScale;
	Mesh->RelativeRotation.Yaw += DeltaSeconds * RotateSpeed;
	Mesh->UpdateComponentToWorld();
}
