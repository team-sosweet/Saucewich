// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Projectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ForceFeedbackAttenuation.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"

#include "SauceMarker.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "SaucewichGameMode.h"

AProjectile::AProjectile()
	: Mesh{CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"))},
	  Movement{CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"))}
{
	RootComponent = Mesh;
	InitialLifeSpan = 5;
	Mesh->SetCollisionProfileName(TEXT("Projectile"));
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

	if (const auto PSC = UGameplayStatics::SpawnEmitterAtLocation(
		World, ImpactFX.LoadSynchronous(),
		Location, FRotator::ZeroRotator,
		true, EPSCPoolMethod::AutoRelease
	))
	{
		PSC->SetColorParameter(TEXT("Color"), GetColor());
	}

	UGameplayStatics::SpawnForceFeedbackAtLocation(
		World, ForceFeedbackEffect.LoadSynchronous(), Location,
		FRotator::ZeroRotator, false, 1.f, 0.f,
		ForceFeedbackAttenuation.LoadSynchronous()
	);

	ASauceMarker::Add(Team, GetSauceMarkScale(), Hit, this);
#endif

	Release();
}

void AProjectile::OnRep_Team() const
{
	if (IsTeamValid())
	{
		const auto Idx = Mesh->GetMaterialIndex(TEXT("TeamColor"));
		auto Mat = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(Idx));
		if (!Mat) Mat = Mesh->CreateDynamicMaterialInstance(Idx);
		Mat->SetVectorParameterValue(TEXT("Color"), GetColor());
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
	return ASaucewichGameMode::GetData(this).Teams[Team].Color;
}
