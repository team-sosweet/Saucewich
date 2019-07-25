// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Gun.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealNetwork.h"
#include "Projectile.h"
#include "ProjectilePoolComponent.h"
#include "TpsCharacter.h"
#include "WeaponComponent.h"

AGun::AGun()
	:ProjectilePool{CreateDefaultSubobject<UProjectilePoolComponent>("ProjectilePool")}
{
	ProjectilePool->SetupAttachment(RootComponent, "Muzzle");
}

void AGun::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FireRandSeed = FMath::Rand();
		OnRep_FireRandSeed();
	}
}

void AGun::Tick(const float DeltaSeconds)
{
	bIsGunTraceCacheValid = false;

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
	DOREPLIFETIME_CONDITION(AGun, FireRandSeed, COND_InitialOnly);
}

void AGun::Shoot()
{
	const auto& Transform = ProjectilePool->GetComponentTransform();
	FHitResult Hit;
	const auto Dir = GunTrace(Hit) ? Hit.ImpactPoint - Transform.GetLocation() : Transform.GetRotation().Vector().RotateAngleAxis(PitchOffsetWhileNoTarget, GetActorForwardVector());
	const auto Rotation = FireRand.VRandCone(Dir, HorizontalSpread, VerticalSpread).ToOrientationQuat();
	ProjectilePool->Spawn(Rotation)->SetSpeed(ProjectileSpeed);
}

bool AGun::GunTrace(FHitResult& OutHit)
{
	if (bIsGunTraceCacheValid)
	{
		OutHit = GunTraceCache;
		return bGunTraceResultCache;
	}
	bIsGunTraceCacheValid = true;

	const auto Character = GetCharacter();
	const auto AimRotation = Character->GetBaseAimRotation();

	const auto AimDir = AimRotation.Vector();
	const auto Start = Character->GetPawnViewLocation() + AimDir * (Character->GetSpringArmLength() + TraceStartOffset);
	const auto End = Start + AimDir * MaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	const TArray<AActor*> Ignored{ this, GetOwner() };

	TArray<FHitResult> BoxHits;
	UKismetSystemLibrary::BoxTraceMultiByProfile(this, Start, End, {0.f, TraceBoxSize.X, TraceBoxSize.Y}, AimRotation, PawnOnly.Name, false, Ignored, EDrawDebugTrace::ForOneFrame, BoxHits, false);
	
	/*
	GetWorld()->SweepMultiByProfile(
		BoxHits, Start, End, AimRotation.Quaternion(), PawnOnly.Name,
		FCollisionShape::MakeBox({0.f, TraceBoxSize.X, TraceBoxSize.Y}), Params
	);
	*/

	auto HitPawn = -1;
	for (auto i = 0; i < BoxHits.Num(); ++i)
	{
		//if (!GetWorld()->LineTraceTestByProfile(BoxHits[i].Location, Start, NoPawn.Name, Params))
		FHitResult a;
		if (!UKismetSystemLibrary::LineTraceSingleByProfile(this, BoxHits[i].ImpactPoint, Start, NoPawn.Name, false, Ignored, EDrawDebugTrace::ForOneFrame, a, false))
		{
			HitPawn = i;
			break;
		}
	}

	if (HitPawn != -1)
	{
		OutHit = GunTraceCache = BoxHits[HitPawn];
		return bGunTraceResultCache = true;
	}

	const auto Profile = GetDefault<AProjectile>(ProjectilePool->GetProjectileClass())->GetCollisionProfile();
	// bGunTraceResultCache = GetWorld()->LineTraceSingleByProfile(GunTraceCache, Start, End, Profile, Params);
	bGunTraceResultCache = UKismetSystemLibrary::LineTraceSingleByProfile(this, Start, End, Profile, false, Ignored, EDrawDebugTrace::ForOneFrame, GunTraceCache, false);
	OutHit = GunTraceCache;
	return bGunTraceResultCache;
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

void AGun::OnRep_FireRandSeed()
{
	FireRand.Initialize(FireRandSeed);
}
