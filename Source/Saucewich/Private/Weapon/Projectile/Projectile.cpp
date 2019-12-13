// Copyright 2019 Othereum. All Rights Reserved.

#include "Weapon/Projectile/Projectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ForceFeedbackAttenuation.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "Entity/SauceMarker.h"
#include "GameMode/SaucewichGameState.h"
#include "GameMode/SaucewichGameMode.h"
#include "Player/TpsCharacter.h"
#include "UserSettings.h"
#include "Names.h"

AProjectile::AProjectile()
	: Mesh{CreateDefaultSubobject<UStaticMeshComponent>(Names::Mesh)},
	  Movement{CreateDefaultSubobject<UProjectileMovementComponent>(Names::Movement)}
{
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(Names::Projectile);
}

void AProjectile::ResetSpeed() const
{
	SetSpeed(Movement->InitialSpeed);
}

void AProjectile::SetSpeed(const float Speed) const
{
	Movement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
}

void AProjectile::Explode(const FHitResult& Hit)
{
	if (HasAuthority() && CanExplode(Hit))
	{
		MulticastExplode(Hit);
	}
}

bool AProjectile::CanExplode(const FHitResult& Hit) const
{
	return IsTeamValid();
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	GS->AddDilatableActor(this);
}

void AProjectile::OnActivated()
{
	if (HasAuthority())
	{
		Team = CastChecked<ATpsCharacter>(GetInstigator())->GetTeam();
		OnRep_Team();
	}

	Movement->SetUpdatedComponent(Mesh);
}

void AProjectile::OnReleased()
{
	Movement->SetUpdatedComponent(nullptr);
	Team = -1;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, Team);
}

void AProjectile::OnExplode(const FHitResult& Hit)
{
#if !UE_SERVER
	const auto World = GetWorld();
	const auto Location = GetActorLocation();

	if (ImpactSounds.Num() > 0)
	{
		UGameplayStatics::PlaySoundAtLocation(
			World,
			ImpactSounds[FMath::RandHelper(ImpactSounds.Num())].LoadSynchronous(),
			Location
		);
	}

	if (const auto FX = ImpactFX.LoadSynchronous())
	{
		const auto PSC = UGameplayStatics::SpawnEmitterAtLocation(
			World, FX, Location, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease
		);
		const auto GS = CastChecked<ASaucewichGameState>(World->GetGameState());
		GS->AddDilatablePSC(PSC);
		PSC->SetColorParameter(Names::Color, GetColor());
	}

	if (UUserSettings::Get()->bVibration)
	{
		UGameplayStatics::SpawnForceFeedbackAtLocation(
			World, ForceFeedbackEffect.LoadSynchronous(), Location,
			FRotator::ZeroRotator, false, 1.f, 0.f,
			ForceFeedbackAttenuation.LoadSynchronous()
		);
	}

	ASauceMarker::Add(Team, GetSauceMarkScale(), Hit, this);
#endif

	Release();
}

void AProjectile::OnRep_Team() const
{
	if (IsTeamValid())
	{
		const auto Idx = Mesh->GetMaterialIndex(Names::TeamColor);
		auto Mat = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(Idx));
		if (!Mat) Mat = Mesh->CreateDynamicMaterialInstance(Idx);
		Mat->SetVectorParameterValue(Names::Color, GetColor());
	}
}

void AProjectile::MulticastExplode_Implementation(const FHitResult& Hit)
{
	if (CanExplode(Hit)) OnExplode(Hit);
}

FName AProjectile::GetCollisionProfile() const
{
	return Mesh->GetCollisionProfileName();
}

FLinearColor AProjectile::GetColor() const
{
	auto&& Teams = ASaucewichGameMode::GetData(this).Teams;
	return ensure(Teams.IsValidIndex(Team)) ? Teams[Team].Color : FLinearColor{};
}
