// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Gun.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealNetwork.h"
#include "ActorPool.h"
#include "GunProjectile.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "WeaponComponent.h"

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
	Super::Tick(DeltaSeconds);

	if (bFiring)
	{
		const auto CurTime = GetGameTimeSinceCreation();
		const auto Delay = 60.f / GetData<FGunData>(FILE_LINE_FUNC)->Rpm;
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
	if (!GetPool()) return;

	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	if (!Data) return;

	const auto MuzzleTransform = GetMesh()->GetSocketTransform("Muzzle");
	const auto MuzzleLocation = MuzzleTransform.GetLocation();
	GUARANTEE_MSG(!MuzzleLocation.IsNearlyZero(), "무기 Muzzle 소켓 설정 안 됨");
	
	FHitResult Hit;
	const auto HitType = GunTrace(Hit);

	if (HitType == EGunTraceHit::Pawn)
	{
		const auto ShotDir = (Hit.ImpactPoint - MuzzleLocation).GetSafeNormal();
		Hit.GetActor()->TakeDamage(
			Data->Damage,
			FPointDamageEvent{Data->Damage, Hit, ShotDir, Data->DamageType},
			GetInstigator()->GetController(),
			this
		);
	}

	const auto Dir =
		HitType != EGunTraceHit::None ? Hit.ImpactPoint - MuzzleLocation
		: MuzzleTransform.GetRotation().Vector();

	const auto Rotation = FireRand.VRandCone(Dir, Data->HorizontalSpread, Data->VerticalSpread).ToOrientationQuat();

	FTransform SpawnTransform{
		Rotation, MuzzleLocation, FVector{FireRand.FRandRange(Data->MinProjectileSize, Data->MaxProjectileSize)}
	};

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.Instigator = GetInstigator();

	if (const auto Projectile = GetPool()->Spawn<AGunProjectile>(*Data->ProjectileClass, SpawnTransform, Parameters))
	{
		Projectile->bCosmetic = HitType == EGunTraceHit::Pawn;
		Projectile->SetColor(GetColor());
	}

	LastClip = --Clip;
	bDried = Clip == 0;
	ReloadAlpha = 0.f;
	ReloadWaitingTime = 0.f;
}

EGunTraceHit AGun::GunTrace(FHitResult& OutHit) const
{
	const auto Character = Cast<ATpsCharacter>(GetOwner());
	if (!Character->IsValidLowLevel()) return EGunTraceHit::None;

	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	if (!Data) return EGunTraceHit::None;

	const auto AimRotation = Character->GetBaseAimRotation();
	const auto AimDir = AimRotation.Vector();
	const auto Start = Character->GetSpringArmLocation() + AimDir * Data->TraceStartOffset;
	const auto End = Start + AimDir * Data->MaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(TArray<AActor*>{GetWorld()->GetGameState<ASaucewichGameState>()->GetCharactersByTeam(Character->GetTeam())});

	TArray<FHitResult> BoxHits;
	GetWorld()->SweepMultiByProfile(
		BoxHits, Start, End, AimRotation.Quaternion(), Data->PawnOnly.Name,
		FCollisionShape::MakeBox({0.f, Data->TraceBoxSize.X, Data->TraceBoxSize.Y}), Params
	);

	auto HitPawn = -1;
	for (auto i = 0; i < BoxHits.Num(); ++i)
	{
		const auto Chr = Cast<ATpsCharacter>(BoxHits[i].GetActor());
		if (!Chr || Chr->IsInvincible()) continue;

		if (!GetWorld()->LineTraceTestByProfile(BoxHits[i].ImpactPoint, Start, Data->NoPawn.Name, Params))
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

	const auto Profile = GetDefault<AGunProjectile>(Data->ProjectileClass)->GetCollisionProfile();
	return GetWorld()->LineTraceSingleByProfile(OutHit, Start, End, Profile, Params) ? EGunTraceHit::Other : EGunTraceHit::None;
}

const FGunData& AGun::GetGunData() const
{
	static const FGunData Default{};
	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	return Data ? *Data : Default;
}

void AGun::FireP()
{
	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	if (!Data) return;
	
	bFiring = true;
	FireLag = 0.f;
	if (LastFire + 60.f / Data->Rpm <= GetGameTimeSinceCreation())
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
	if (const auto Character = Cast<ATpsCharacter>(GetOwner()))
		if (const auto Data = GetData(FILE_LINE_FUNC))
			Character->GetWeaponComponent()->TrySelectWeapon(Data->Slot);
}

void AGun::OnActivated()
{
	Super::OnActivated();
	
	if (const auto Data = GetData<FGunData>(FILE_LINE_FUNC))
	{
		Clip = Data->ClipSize;
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

void AGun::OnReleased()
{
	Super::OnReleased();
	bFiring = false;
	FireLag = 0.f;
	LastFire = 0.f;
	bDried = false;
	ReloadWaitingTime = 0.f;
	ReloadAlpha = 0.f;
}

bool AGun::CanFire() const
{
	return IsActive() && Clip > 0 && !bDried;
}

void AGun::Reload(const float DeltaSeconds)
{
	if (!HasAuthority()) return;

	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	if (!Data) return;

	if (Clip < Data->ClipSize)
	{
		if (ReloadWaitingTime >= (bDried ? Data->ReloadWaitTimeAfterDried : Data->ReloadWaitTime))
		{
			ReloadAlpha = FMath::Clamp(ReloadAlpha + DeltaSeconds / Data->ReloadTime, 0.f, 1.f);
			Clip = FMath::CubicInterp<float>(LastClip, 0.f, Data->ClipSize, 0.f, ReloadAlpha);

			if (bDried && Clip >= Data->MinClipToFireAfterDried)
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
