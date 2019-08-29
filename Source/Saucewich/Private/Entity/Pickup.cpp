// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Pickup.h"

#include "Components/SphereComponent.h"

#include "Entity/PickupSpawner.h"
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

	for (auto&& OverlapInfo : Collision->GetOverlapInfos())
	{
		if (const auto Other = Cast<APickup>(OverlapInfo.OverlapInfo.Actor.Get()))
		{
			auto Force = Other->GetActorLocation();
			Force -= GetActorLocation();
			
			auto NewSize = PushStrength;
			const auto SizeSqr2D = Force.SizeSquared2D();
			if (SizeSqr2D <= SMALL_NUMBER) Force = FMath::VRand();
			else NewSize *= FMath::InvSqrt(SizeSqr2D);
			
			Force.X *= NewSize;
			Force.Y *= NewSize;
			Force.Z = 0;
			
			Other->Collision->AddForce(Force, NAME_None, true);
		}
	}

	if (PickingActor)
	{
		if (CanPickedUp(PickingActor))
		{
			if (PickingTimer == 0) StartPickUp(PickingActor);
			if ((PickingTimer += DeltaSeconds) >= PickupTime)
			{
				BePickedUp(PickingActor);
			}
		}
		else if (PickingTimer != 0)
		{
			PickingTimer = 0;
			CancelPickUp(PickingActor);
		}
	}
}

void APickup::NotifyActorBeginOverlap(AActor* const OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (CanEverPickedUp(OtherActor))
	{
		if (PickupTime <= 0)
		{
			if (CanPickedUp(OtherActor)) BePickedUp(OtherActor);
		}
		else if (!PickingActor)
		{
			PickingActor = OtherActor;
		}
	}
}

void APickup::NotifyActorEndOverlap(AActor* const OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (PickingActor == OtherActor)
	{
		if (PickingTimer != 0) CancelPickUp(PickingActor);
		PickingActor = nullptr;
		PickingTimer = 0;
	}
}

void APickup::OnActivated()
{
	Collision->CreatePhysicsState();
	if (HasAuthority()) MulticastSetLocation(GetActorLocation());
}

void APickup::OnReleased()
{
	bSpawnedFromSpawner = false;
	PickingActor = nullptr;
	PickingTimer = 0;
	Collision->DestroyPhysicsState();
}

void APickup::BePickedUp(AActor* By)
{
	if (bSpawnedFromSpawner)
		if (const auto Spawner = Cast<APickupSpawner>(GetOwner()))
			Spawner->PickedUp();
	Release();
}

void APickup::MulticastSetLocation_Implementation(const FVector Location)
{
	if (!HasAuthority()) SetActorLocation(Location);
}
