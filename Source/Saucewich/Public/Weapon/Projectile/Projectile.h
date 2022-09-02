// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
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

	FLinearColor GetColor() const;
	UStaticMeshComponent* GetMesh() const { return Mesh; }
	UProjectileMovementComponent* GetMovement() const { return Movement; }
	bool IsTeamValid() const { return Team != static_cast<decltype(Team)>(-1); }

	UFUNCTION(BlueprintCallable)
	void Explode(const FHitResult& Hit);
	virtual bool CanExplode(const FHitResult& Hit) const;

protected:
	void BeginPlay() override;
	void OnActivated() override;
	void OnReleased() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetSauceMarkScale() const { return 1.f; }
	virtual void OnExplode(const FHitResult& Hit);

	UFUNCTION()
	void OnRep_Team() const;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastExplode(const FHitResult& Hit);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UProjectileMovementComponent* Movement;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<USoundBase>> ImpactSounds;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<USoundAttenuation> ImpactSoundAttenuation;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UParticleSystem> ImpactFX;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UForceFeedbackEffect> ForceFeedbackEffect;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UForceFeedbackAttenuation> ForceFeedbackAttenuation;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_Team, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team = -1;

	UPROPERTY(EditDefaultsOnly)
	uint8 bSauceMark : 1;

	UPROPERTY(EditDefaultsOnly)
	uint8 bVolumeByScale : 1;
};
