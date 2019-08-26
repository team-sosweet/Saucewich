// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Pickup.h"

#include "Components/SphereComponent.h"
#include "TimerManager.h"

#include "ShadowComponent.h"

APickup::APickup()
	:Collision{CreateDefaultSubobject<USphereComponent>("Collision")},
	Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")},
	Shadow{CreateDefaultSubobject<UShadowComponent>("Shadow")}
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = Collision;
	Collision->SetIsReplicated(true);
	Collision->BodyInstance.bLockXRotation = true;
	Collision->BodyInstance.bLockYRotation = true;
	Collision->BodyInstance.bLockZRotation = true;
	Collision->BodyInstance.bSimulatePhysics = true;
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

	TArray<AActor*> Actors;
	GetOverlappingActors(Actors, StaticClass());
	for (const auto Actor : Actors)
	{
		auto Force = Actor->GetActorLocation();
		Force -= GetActorLocation();
		auto NewSize = PushStrength;
		const auto SizeSqr2D = Force.SizeSquared2D();
		if (SizeSqr2D <= SMALL_NUMBER)
		{
			Force = FMath::VRand();
		}
		else
		{
			NewSize *= FMath::InvSqrt(SizeSqr2D);
		}
		Force.X *= NewSize;
		Force.Y *= NewSize;
		Force.Z = 0;
		static_cast<APickup*>(Actor)->Collision->AddForce(Force, NAME_None, true);
	}
}

void APickup::OnReleased()
{
	Collision->DestroyPhysicsState();
}

void APickup::OnActivated()
{
	Collision->CreatePhysicsState();
	if (HasAuthority()) MulticastSetLocation(GetActorLocation());
}

void APickup::MulticastSetLocation_Implementation(const FVector Location)
{
	if (!HasAuthority()) SetActorLocation(Location);
}
