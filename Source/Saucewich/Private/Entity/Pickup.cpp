// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Entity/Pickup.h"

#include "Components/SphereComponent.h"

#include "Entity/PickupSpawner.h"
#include "Player/TpsCharacter.h"
#include "ShadowComponent.h"
#include "GameMode/SaucewichGameState.h"
#include "Names.h"

APickup::APickup()
	:Collision{CreateDefaultSubobject<USphereComponent>(Names::Collision)},
	Mesh{CreateDefaultSubobject<UStaticMeshComponent>(Names::Mesh)},
	Shadow{CreateDefaultSubobject<UShadowComponent>(Names::Shadow)}
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = Collision;
	Collision->SetIsReplicated(true);
	Collision->BodyInstance.bLockXRotation = true;
	Collision->BodyInstance.bLockYRotation = true;
	Collision->BodyInstance.bLockZRotation = true;
	Collision->BodyInstance.bSimulatePhysics = true;
	Collision->BodyInstance.SetCollisionProfileNameDeferred(Names::Pickup);
	
	Mesh->SetupAttachment(Collision);
	Mesh->BodyInstance.SetCollisionProfileNameDeferred(Names::NoCollision);
	Mesh->SetGenerateOverlapEvents(true);
	
	Shadow->SetupAttachment(Mesh);
	Shadow->SetRelativeScale3D(FVector{Collision->GetScaledSphereRadius() / 50.f});
}

void APickup::Freeze()
{
	SetActorTickEnabled(false);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	GS->AddDilatableActor(this);
}

void APickup::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Time += DeltaSeconds;

	auto NewLocation = Mesh->GetRelativeLocation();
	NewLocation.Z = FMath::Sin(Time * BounceSpeed) * BounceScale;

	Mesh->SetRelativeLocation(NewLocation);
	Mesh->AddRelativeRotation(FRotator{ 0.f, DeltaSeconds * RotateSpeed, 0.f });

	Mesh->UpdateComponentToWorld();

	for (auto&& OverlapInfo : Collision->GetOverlapInfos())
	{
		if (const auto Other = Cast<APickup>(OverlapInfo.OverlapInfo.GetActor()))
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

	if (PickingChar)
	{
		if (CanPickedUp(PickingChar))
		{
			if (PickingTimer == 0) StartPickUp(PickingChar);
			if ((PickingTimer += DeltaSeconds) >= PickupTime)
			{
				BePickedUp(PickingChar);
			}
		}
		else if (PickingTimer != 0)
		{
			PickingTimer = 0;
			CancelPickUp(PickingChar);
		}
	}
}

void APickup::NotifyActorBeginOverlap(AActor* const OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (const auto OtherChar = Cast<ATpsCharacter>(OtherActor))
	{
		if (CanEverPickedUp(OtherChar))
		{
			if (PickupTime <= 0)
			{
				if (CanPickedUp(OtherChar)) BePickedUp(OtherChar);
			}
			else if (!PickingChar)
			{
				PickingChar = OtherChar;
			}
		}
	}
}

void APickup::NotifyActorEndOverlap(AActor* const OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	if (PickingChar == OtherActor)
	{
		if (PickingTimer != 0) CancelPickUp(PickingChar);
		PickingChar = nullptr;
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
	PickingChar = nullptr;
	PickingTimer = 0;
	Collision->DestroyPhysicsState();
}

void APickup::BePickedUp(ATpsCharacter* const By)
{
	OnPickedUp(By);
	
	if (!HasAuthority()) return;

	if (bSpawnedFromSpawner)
		if (const auto Spawner = Cast<APickupSpawner>(GetOwner()))
			Spawner->PickedUp();
	
	Release();
}

void APickup::OnPickedUp_Implementation(ATpsCharacter* By)
{
}

void APickup::StartPickUp_Implementation(ATpsCharacter* const By)
{
	By->OnPickupStarted.Broadcast(PickupTime);
}

void APickup::CancelPickUp_Implementation(ATpsCharacter* const By)
{
	By->OnPickupCanceled.Broadcast();
}

bool APickup::CanEverPickedUp_Implementation(const ATpsCharacter* By) const
{
	return true;
}

bool APickup::CanPickedUp_Implementation(const ATpsCharacter* By) const
{
	return true;
}

void APickup::MulticastSetLocation_Implementation(const FVector Location)
{
	if (!HasAuthority()) SetActorLocation(Location);
}
