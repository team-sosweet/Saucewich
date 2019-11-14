// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "PoolActor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;
class UForceFeedbackEffect;
class UForceFeedbackAttenuation;
class UProjectileMovementComponent;

UCLASS(Abstract)
class AProjectile : public APoolActor
{
	GENERATED_BODY()

public:
	AProjectile();
	void ResetSpeed() const;
	void SetSpeed(float Speed) const;
	FName GetCollisionProfile() const;

	uint8 GetTeam() const;
	FLinearColor GetColor() const;
	UStaticMeshComponent* GetMesh() const { return Mesh; }

protected:
	void OnActivated() override;
	void OnReleased() override;

	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual float GetSauceMarkScale() const { return 1.f; }

private:
	int32 GetMatIdx() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UProjectileMovementComponent* Movement;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<USoundBase>> ImpactSounds;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UParticleSystem> ImpactFX;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UForceFeedbackEffect> ForceFeedbackEffect;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UForceFeedbackAttenuation> ForceFeedbackAttenuation;
};
