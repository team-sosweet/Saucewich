// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Weapon/Gun.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"

#include "Entity/ActorPool.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Weapon/WeaponComponent.h"
#include "Weapon/Projectile/GunProjectile.h"
#include "UserSettings.h"
#include "Names.h"

AGun::AGun()
	:FirePSC{CreateDefaultSubobject<UParticleSystemComponent>(Names::FirePSC)}
{
	PrimaryActorTick.bCanEverTick = true;

	FirePSC->SetupAttachment(GetMesh(), Names::Muzzle);
	FirePSC->bAutoActivate = false;
}

void AGun::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	auto&& Data = GetGunData();
	const auto Delay = 60.f / Data.Rpm;
	if (bFiring)
	{
		for (; FireLag >= Delay; FireLag -= Delay)
		{
			Shoot();
		}
	}
	else
	{
		SpreadAlpha = FMath::Clamp(SpreadAlpha - Data.SpreadDecrease*DeltaSeconds, Data.FirstSpreadRatio, 1.f);
		FirePSC->Deactivate();
	}
	FireLag += DeltaSeconds;
	if (!bFiring && FireLag > Delay) FireLag = Delay;
	
	Reload(DeltaSeconds);
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
	DOREPLIFETIME(AGun, bDried);
	DOREPLIFETIME(AGun, bFiring);
	DOREPLIFETIME(AGun, bFreeze);
}

void AGun::Shoot()
{
	if (!CanFire())
	{
		FirePSC->Deactivate();
		return;
	}

	auto&& Data = GetGunData();

	const auto MuzzleTransform = GetMesh()->GetSocketTransform(Names::Muzzle);
	const auto MuzzleLoc = MuzzleTransform.GetLocation();

	auto&& ActorTransform = GetActorTransform();
	const auto Forward = ActorTransform.GetUnitAxis(EAxis::X);
	const auto Right = ActorTransform.GetUnitAxis(EAxis::Y);

	const auto ProjCls = Data.ProjectileClass.LoadSynchronous();
	const auto Proj = GetDefault<AGunProjectile>(ProjCls);
	const auto ProjColProf = Proj->GetCollisionProfile();

	FHitResult Hit;
	const auto bHit = GunTraceInternal(Hit, ProjColProf, Data);


	const auto Dir = [&]
	{
		if (!bHit) return MuzzleTransform.GetRotation().Vector();

		const auto ProjSpd = Data.ProjectileSpeed;

		const auto PredictGravity = [&](const FVector& To)
		{
			// 이 수치는 근사값인데, 고저차를 반영하지 못하기 때문이다.
			const auto Theta = FMath::Asin(980.f*FVector::Dist(MuzzleLoc, To) / (ProjSpd*ProjSpd)) / 2;
			return (To - MuzzleLoc).GetUnsafeNormal().RotateAngleAxis(FMath::RadiansToDegrees(Theta), Forward);
		};
		
		const auto EnemyVel = Hit.GetActor()->GetVelocity();
		if (EnemyVel.IsNearlyZero()) return PredictGravity(Hit.ImpactPoint);

		const auto EnemyToMuzzle = MuzzleLoc - Hit.ImpactPoint;
		const auto Dot = EnemyToMuzzle.GetUnsafeNormal() | EnemyVel.GetUnsafeNormal();
		const auto Alpha = FMath::Acos(Dot);
		const auto EnemySpd = EnemyVel.Size();
		const auto Theta = FMath::Asin(EnemySpd * FMath::Sin(Alpha) / ProjSpd);
		const auto Time = EnemyToMuzzle.Size() / (EnemySpd*FMath::Cos(Alpha) + ProjSpd*FMath::Cos(Theta));
		const auto PredictedHitLocation = Hit.ImpactPoint + EnemyVel * Time;
		return PredictGravity(PredictedHitLocation);
	}();

	const auto V = 45.f * Data.VerticalSpread;
	const auto H = 45.f * Data.HorizontalSpread;
	
	FTransform SpawnTransform{
		FQuat::Identity, MuzzleLoc,
		FVector{FireRand.FRandRange(Data.MinProjectileSize, Data.MaxProjectileSize)}
	};

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.Instigator = GetInstigator();

	for (auto i = 0; i < Data.NumProjectile; ++i)
	{
		const auto VR = FireRand.FRandRange(-V, V) * SpreadAlpha;
		const auto HR = FireRand.FRandRange(-H, H) * SpreadAlpha;
		SpawnTransform.SetRotation(Dir.RotateAngleAxis(VR, Forward).RotateAngleAxis(HR, Right).ToOrientationQuat());
		AActorPool::Get(this)->Spawn<AGunProjectile>(ProjCls, SpawnTransform, Parameters);
		SpreadAlpha = FMath::Min(SpreadAlpha + Data.SpreadIncrease, 1.f);
	}

	LastClip = --Clip;
	if (!bDried && Clip == 0 && HasAuthority())
	{
		bDried = true;
		OnRep_Dried();
	}
	ReloadAlpha = 0.f;
	ReloadWaitingTime = 0.f;

	UGameplayStatics::PlaySoundAtLocation(this, Data.FireSound.LoadSynchronous(), MuzzleLoc);

	const auto PC = Cast<APlayerController>(GetInstigatorController());
	if (PC && PC->IsLocalController())
	{
		if (UUserSettings::Get(this)->bVibration)
			PC->PlayDynamicForceFeedback(Data.FBBIntensity, Data.FBBDuration, true, false, true, false);

		PC->ClientPlayCameraShake(Data.FireShake.LoadSynchronous(), Data.Recoil);
	}

	FirePSC->Activate();

	OnShoot();
}

bool AGun::GunTrace(FHitResult& OutHit)
{
	auto& Data = GetGunData();
	const auto Cls = Data.ProjectileClass.LoadSynchronous();
	const auto Def = GetDefault<AProjectile>(Cls);
	const auto Profile = Def->GetCollisionProfile();
	return GunTraceInternal(OutHit, Profile, Data);
}

bool AGun::GunTraceInternal(FHitResult& OutHit, const FName ProjColProf, const FGunData& Data)
{
	const auto Character = CastChecked<ATpsCharacter>(GetOwner(), ECastCheckedType::NullAllowed);
	if (!IsValid(Character)) return false;
	
	const auto AimRotation = Character->GetBaseAimRotation();
	const auto AimDir = AimRotation.Vector();
	const auto Start = Character->GetSpringArmLocation() + AimDir * 10.f;
	const auto Offset = AimDir * Data.MaxDistance;
	const auto End = Start + Offset;

	FCollisionQueryParams Params;
	if (const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState(), ECastCheckedType::NullAllowed))
	{
		Params.AddIgnoredActors(TArray<AActor*>{GS->GetCharactersByTeam(Character->GetTeam())});
	}

	TArray<FHitResult> BoxHits;
	GetWorld()->SweepMultiByProfile(
		BoxHits, Start, End, AimRotation.Quaternion(), NAME("PawnOnly"),
		FCollisionShape::MakeBox({ 0.f, Data.TraceBoxSize.X, Data.TraceBoxSize.Y }), Params
	);

	auto HitPawn = -1;
	for (auto i = 0; i < BoxHits.Num(); ++i)
	{
		const auto Chr = Cast<APawn>(BoxHits[i].GetActor());
		if (!Chr || !Chr->ShouldTakeDamage(Data.Damage, FPointDamageEvent{
			Data.Damage, BoxHits[i], (End-Start).GetSafeNormal(), Data.DamageType.LoadSynchronous()
		}, GetInstigatorController(), this)) continue;

		if (!GetWorld()->LineTraceTestByProfile(BoxHits[i].ImpactPoint, Start, NAME("NoPawn"), Params))
		{
			HitPawn = i;
			break;
		}
	}

	if (HitPawn != -1)
	{
		OutHit = BoxHits[HitPawn];
		return true;
	}

	return false;
}

void AGun::OnRep_Dried() const
{
	OnAvailabilityChanged(bDried);
}

const FGunData& AGun::GetGunData() const
{
	return GetData<FGunData>();
}

void AGun::BeginPlay()
{
	Super::BeginPlay();


	const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	GS->AddDilatableActor(this);

	GS->AddDilatablePSC(FirePSC);
	FirePSC->SetFloatParameter(Names::RPM, GetData<FGunData>().Rpm);
}

void AGun::FireP()
{
	const auto Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->IsLocallyControlled())
	{
		const auto Seed = FMath::Rand();
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
		Character->GetWeaponComponent()->TrySelectWeapon(GetData().Slot);
}

void AGun::OnActivated()
{
	Super::OnActivated();
	Clip = GetGunData().ClipSize;
}

void AGun::OnReleased()
{
	Super::OnReleased();
	bFiring = false;
	FireLag = 0.f;
	bDried = false;
	ReloadWaitingTime = 0.f;
}

void AGun::SetColor(const FLinearColor& NewColor)
{
	Super::SetColor(NewColor);
	FirePSC->SetColorParameter(Names::Color, NewColor);
}

void AGun::StartFire(const int32 RandSeed)
{
	FireRand.Initialize(RandSeed);
	bFiring = true;
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
	return !bFreeze && IsActive() && Clip > 0 && !bDried;
}

void AGun::Reload(const float DeltaSeconds)
{
	if (!HasAuthority()) return;

	auto&& Data = GetGunData();

	if (Clip < Data.ClipSize)
	{
		if (ReloadWaitingTime >= (bDried ? Data.ReloadWaitTimeAfterDried : Data.ReloadWaitTime))
		{
			ReloadAlpha = FMath::Clamp(ReloadAlpha + DeltaSeconds / Data.ReloadTime, 0.f, 1.f);
			Clip = FMath::CubicInterp<float>(LastClip, 0.f, Data.ClipSize, 0.f, ReloadAlpha);

			if (bDried && Clip >= Data.MinClipToFireAfterDried)
			{
				bDried = false;
				OnRep_Dried();
			}
		}
		else
		{
			ReloadWaitingTime += DeltaSeconds;
		}
	}
}
