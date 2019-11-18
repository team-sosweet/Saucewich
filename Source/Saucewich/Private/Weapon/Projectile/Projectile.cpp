// Copyright 2019 Team Sosweet. All Rights Reserved.

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
	: Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")},
	  Movement{CreateDefaultSubobject<UProjectileMovementComponent>("Movement")}
{
	RootComponent = Mesh;
	InitialLifeSpan = 5;
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
	if (CanExplode(Hit)) OnExplode(Hit);
}

bool AProjectile::CanExplode(const FHitResult& Hit) const
{
	return Team != static_cast<uint8>(-1);
}

void AProjectile::OnActivated()
{
	if (!bReplicates || HasAuthority())
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

	Release();
}

void AProjectile::OnRep_Team() const
{
	if (Team == static_cast<uint8>(-1)) return;
	
	static TArray<TWeakObjectPtr<UMaterialInstanceDynamic>> Materials;
	Materials.SetNum(ASaucewichGameMode::GetData(this).Teams.Num());

	auto& MatPtr = Materials[Team];
	if (auto Mat = MatPtr.Get())
	{
		Mesh->SetMaterial(GetMatIdx(), Mat);
	}
	else
	{
		Mat = Mesh->CreateDynamicMaterialInstance(GetMatIdx());
		Mat->SetVectorParameterValue(TEXT("Color"), GetColor());
		MatPtr = Mat;
	}
}

int32 AProjectile::GetMatIdx() const
{
	return Mesh->GetMaterialIndex(TEXT("TeamColor"));
}

FName AProjectile::GetCollisionProfile() const
{
	return Mesh->GetCollisionProfileName();
}

FLinearColor AProjectile::GetColor() const
{
	return ASaucewichGameMode::GetData(this).Teams[Team].Color;
}
