// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Gun.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "Online/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Weapon/GunSharedData.h"
#include "Weapon/WeaponComponent.h"
#include "Weapon/Projectile/GunProjectile.h"

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
}

FVector AGun::VRandCone(const FVector& Dir, const float HorizontalConeHalfAngleRad, const float VerticalConeHalfAngleRad)
{
	if (VerticalConeHalfAngleRad > 0 && HorizontalConeHalfAngleRad > 0)
	{
		std::uniform_real_distribution<float> Distribution;
		const auto Rand = [this, &Distribution]{return Distribution(FireRand);};
		
		const auto RandU = Rand();
		const auto RandV = Rand();

		const auto Theta = 2.f * PI * RandU;
		auto Phi = FMath::Acos(2.f * RandV - 1.f);

		auto ConeHalfAngleRad = FMath::Square(FMath::Cos(Theta) / VerticalConeHalfAngleRad) + FMath::Square(FMath::Sin(Theta) / HorizontalConeHalfAngleRad);
		ConeHalfAngleRad = FMath::Sqrt(1.f / ConeHalfAngleRad);

		Phi = FMath::Fmod(Phi, ConeHalfAngleRad);

		const FMatrix DirMat = FRotationMatrix(Dir.Rotation());
		const auto DirZ = DirMat.GetScaledAxis(EAxis::X);
		const auto DirY = DirMat.GetScaledAxis(EAxis::Y);

		auto Result = Dir.RotateAngleAxis(Phi * 180.f / PI, DirY);
		Result = Result.RotateAngleAxis(Theta * 180.f / PI, DirZ);

		Result = Result.GetSafeNormal();

		return Result;
	}
	return Dir.GetSafeNormal();
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

	const auto Rotation = VRandCone(Dir, Data->HorizontalSpread, Data->VerticalSpread).ToOrientationQuat();

	FTransform SpawnTransform{
		Rotation, MuzzleLocation, FVector{std::uniform_real_distribution<float>{Data->MinProjectileSize, Data->MaxProjectileSize}(FireRand)}
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
	const auto Shared = GetSharedData<UGunSharedData>();
	if (!GUARANTEE(Shared != nullptr)) return EGunTraceHit::None;
	
	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	if (!Data) return EGunTraceHit::None;

	const auto Character = Cast<ATpsCharacter>(GetOwner());
	if (!Character->IsValidLowLevel()) return EGunTraceHit::None;

	const auto AimRotation = Character->GetBaseAimRotation();
	const auto AimDir = AimRotation.Vector();
	const auto Start = Character->GetSpringArmLocation() + AimDir * Shared->TraceStartOffset;
	const auto End = Start + AimDir * Data->MaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActors(TArray<AActor*>{GetWorld()->GetGameState<ASaucewichGameState>()->GetCharactersByTeam(Character->GetTeam())});

	TArray<FHitResult> BoxHits;
	GetWorld()->SweepMultiByProfile(
		BoxHits, Start, End, AimRotation.Quaternion(), Shared->PawnOnly.Name,
		FCollisionShape::MakeBox({0.f, Data->TraceBoxSize.X, Data->TraceBoxSize.Y}), Params
	);

	auto HitPawn = -1;
	for (auto i = 0; i < BoxHits.Num(); ++i)
	{
		const auto Chr = Cast<ATpsCharacter>(BoxHits[i].GetActor());
		if (!Chr || Chr->IsInvincible()) continue;

		if (!GetWorld()->LineTraceTestByProfile(BoxHits[i].ImpactPoint, Start, Shared->NoPawn.Name, Params))
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
	const auto Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->IsLocallyControlled())
	{
		static std::default_random_engine Eng{std::random_device{}()};
		const auto Seed = std::uniform_int_distribution<int32>{}(Eng);
		StartFire(Seed);
		ServerStartFire(Seed);
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

void AGun::StartFire(const int32 RandSeed)
{
	const auto Data = GetData<FGunData>(FILE_LINE_FUNC);
	if (!Data) return;

	FireRand.seed(RandSeed);
	
	bFiring = true;
	FireLag = 0.f;
	if (LastFire + 60.f / Data->Rpm <= GetGameTimeSinceCreation())
	{
		Shoot();
		LastFire = GetGameTimeSinceCreation();
	}
}

void AGun::MulticastStartFire_Implementation(const int32 RandSeed)
{
	const auto Pawn = Cast<APawn>(GetOwner());
	if (Pawn && !Pawn->IsLocallyControlled()) StartFire(RandSeed);
}

void AGun::ServerStartFire_Implementation(const int32 RandSeed)
{
	MulticastStartFire(RandSeed);
}

bool AGun::ServerStartFire_Validate(int32)
{
	return true;
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
