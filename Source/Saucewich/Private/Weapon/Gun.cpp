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
		Clip = ClipSize;
	}
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

	Reload(DeltaSeconds);
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
	DOREPLIFETIME(AGun, bDried);
	DOREPLIFETIME(AGun, bFiring);
	DOREPLIFETIME_CONDITION(AGun, FireRandSeed, COND_InitialOnly);
}

void AGun::Shoot()
{
	if (!CanFire()) return;

	const auto& Transform = ProjectilePool->GetComponentTransform();
	FHitResult Hit;
	const auto HitType = GunTrace(Hit);

	if (HitType == EGunTraceHit::Pawn)
	{
		const auto ShotDir = (Hit.ImpactPoint - ProjectilePool->GetComponentLocation()).GetSafeNormal();
		Hit.GetActor()->TakeDamage(
			Damage,
			FPointDamageEvent{Damage, Hit, ShotDir, DamageType},
			GetInstigator()->GetController(),
			this
		);
	}

	const auto Dir =
		HitType != EGunTraceHit::None ? Hit.ImpactPoint - Transform.GetLocation()
		: Transform.GetRotation().Vector();

	const auto Rotation = FireRand.VRandCone(Dir, HorizontalSpread, VerticalSpread).ToOrientationQuat();

	ProjectilePool->Spawn(Rotation, HitType == EGunTraceHit::Pawn);

	LastClip = --Clip;
	bDried = Clip == 0;
	ReloadAlpha = 0.f;
	ReloadWaitingTime = 0.f;
}

EGunTraceHit AGun::GunTrace(FHitResult& OutHit)
{
	const auto Character = GetCharacter();
	const auto AimRotation = Character->GetBaseAimRotation();

	const auto AimDir = AimRotation.Vector();
	const auto Start = Character->GetPawnViewLocation() + AimDir * (Character->GetSpringArmLength() + TraceStartOffset);
	const auto End = Start + AimDir * MaxDistance;

	/*
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	*/

	const TArray<AActor*> Ignored{ GetOwner() };
	const auto Debug = Character->IsLocallyControlled() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;

	TArray<FHitResult> BoxHits;
	UKismetSystemLibrary::BoxTraceMultiByProfile(this, Start, End, {0.f, TraceBoxSize.X, TraceBoxSize.Y}, AimRotation, PawnOnly.Name, false, Ignored, Debug, BoxHits, false);
	
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
		if (!UKismetSystemLibrary::LineTraceSingleByProfile(this, BoxHits[i].ImpactPoint, Start, NoPawn.Name, false, Ignored, Debug, a, false))
		{
			HitPawn = i;
			break;
		}
	}

	if (HitPawn != -1)
	{
		OutHit = BoxHits[HitPawn];
		return EGunTraceHit::Pawn;
	}

	const auto Profile = GetDefault<AProjectile>(ProjectilePool->GetProjectileClass())->GetCollisionProfile();
	// const auto bHit = GetWorld()->LineTraceSingleByProfile(GunTraceCache, Start, End, Profile, Params);
	return UKismetSystemLibrary::LineTraceSingleByProfile(this, Start, End, Profile, false, Ignored, Debug, OutHit, false) ? EGunTraceHit::Other : EGunTraceHit::None;
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

bool AGun::CanFire() const
{
	return Clip > 0 && !bDried;
}

void AGun::Reload(const float DeltaSeconds)
{
	if (!HasAuthority()) return;

	if (Clip < ClipSize)
	{
		if (ReloadWaitingTime >= (bDried ? ReloadWaitTimeAfterDried : ReloadWaitTime))
		{
			ReloadAlpha = FMath::Clamp(ReloadAlpha + DeltaSeconds / ReloadTime, 0.f, 1.f);
			Clip = FMath::CubicInterp<float>(LastClip, 0.f, ClipSize, 0.f, ReloadAlpha);

			if (bDried && Clip >= MinClipToFireAfterDried)
			{
				bDried = false;
			}
		}
		else
		{
			ReloadWaitingTime += DeltaSeconds;
		}
	}
}

void AGun::OnRep_FireRandSeed()
{
	FireRand.Initialize(FireRandSeed);
}
