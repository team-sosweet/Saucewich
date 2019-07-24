// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Gun.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "ProjectilePoolComponent.h"
#include "TpsCharacter.h"
#include "WeaponComponent.h"

AGun::AGun()
	:ProjectilePool{CreateDefaultSubobject<UProjectilePoolComponent>("ProjectilePool")}
{
	ProjectilePool->SetupAttachment(RootComponent, "Muzzle");
}

FHitResult AGun::GunTrace() const
{
	const auto Character = GetCharacter();
	const auto AimRotation = Character->GetBaseAimRotation();

	const auto Start = Character->GetPawnViewLocation();
	const auto End = Start + AimRotation.Vector() * MaxDistance;

	TArray<FHitResult> BoxHits;
	GetWorld()->SweepMultiByProfile(
		BoxHits, Start, End, AimRotation.Quaternion(), PawnOnly.Name,
		FCollisionShape::MakeBox({0.f, TraceBoxSize.X, TraceBoxSize.Y})
	);

	auto HitPawn = -1;
	for (auto i = 0; i < BoxHits.Num(); ++i)
	{
		if (!GetWorld()->LineTraceTestByProfile(BoxHits[i].Location, Start, NoPawn.Name))
		{
			HitPawn = i;
			break;
		}
	}

	if (HitPawn != -1) return BoxHits[HitPawn];

	FHitResult Hit;
	//if (GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ))
	return Hit;
}

void AGun::FireP()
{
	bFiring = true;
	FireLag = 0.f;
	if (LastFire + 60.f / Rpm <= GetGameTimeSinceCreation())
	{
		Shoot();
		LastFire = GetGameTimeSinceCreation();
	}
}

void AGun::FireR()
{
	bFiring = false;
}

void AGun::SlotP()
{
	GetCharacter()->GetWeaponComponent()->TrySelectWeapon(GetSlot());
}

void AGun::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bFiring)
	{
		const auto CurTime = GetGameTimeSinceCreation();
		const auto Delay = 60.f / Rpm;
		for (; FireLag >= Delay; FireLag -= Delay)
		{
			Shoot();
			LastFire = CurTime;
		}
		FireLag += DeltaSeconds;
	}
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
	DOREPLIFETIME(AGun, bFiring);
}
