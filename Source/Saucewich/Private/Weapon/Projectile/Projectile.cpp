// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Projectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ForceFeedbackAttenuation.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "SauceMarker.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "SaucewichGameMode.h"

AProjectile::AProjectile()
	:Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")},
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

void AProjectile::OnActivated()
{
	static TArray<TWeakObjectPtr<UMaterialInstanceDynamic>> Materials;
	Materials.SetNum(ASaucewichGameMode::GetData(this).Teams.Num());

	auto& MatPtr = Materials[GetTeam()];
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
	
	Movement->SetUpdatedComponent(Mesh);
}

void AProjectile::OnReleased()
{
	Movement->SetUpdatedComponent(nullptr);
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	const auto World = GetWorld();

	if (ImpactSounds.Num() > 0)
		UGameplayStatics::PlaySoundAtLocation(
			World, ImpactSounds[FMath::RandHelper(ImpactSounds.Num())].LoadSynchronous(), HitLocation
		);

	UGameplayStatics::SpawnEmitterAtLocation(
		World, ImpactFX.LoadSynchronous(), HitLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease
	)->SetColorParameter(TEXT("Color"), GetColor());

	UGameplayStatics::SpawnForceFeedbackAtLocation(
		World, ForceFeedbackEffect.LoadSynchronous(), HitLocation,
		FRotator::ZeroRotator, false, 1.f, 0.f,
		ForceFeedbackAttenuation.LoadSynchronous()
	);

	ASauceMarker::Add(GetTeam(), GetSauceMarkScale(), Hit, this);
	
	Release();
}

uint8 AProjectile::GetTeam() const
{
	return GetInstigator<ATpsCharacter>()->GetTeam();
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
	return ASaucewichGameMode::GetData(this).Teams[GetTeam()].Color;
}
