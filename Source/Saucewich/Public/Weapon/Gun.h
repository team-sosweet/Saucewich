// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "Weapon/Weapon.h"
#include "Engine/CollisionProfile.h"
#include "Gun.generated.h"

UCLASS(Abstract)
class AGun : public AWeapon
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UProjectilePoolComponent* ProjectilePool;

public:
	AGun();

	UProjectilePoolComponent* GetProjectilePool() const { return ProjectilePool; }

	float GetDamage() const { return Damage; }
	float GetProjectileSpeed() const { return ProjectileSpeed; }
	TSubclassOf<UDamageType> GetDamageType() const { return DamageType; }

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Shoot();

	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit);

	void FireP() override;
	void FireR() override;
	void SlotP() override;
	
private:
	UFUNCTION()
	void OnRep_FireRandSeed();

	FHitResult GunTraceCache;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FCollisionProfileName PawnOnly;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FCollisionProfileName NoPawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FVector2D TraceBoxSize;
	FRandomStream FireRand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float TraceStartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Rpm;
	float FireLag;
	float LastFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float VerticalSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float HorizontalSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float MaxDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ProjectileSpeed;

	UPROPERTY(ReplicatedUsing=OnRep_FireRandSeed, Transient)
	int32 FireRandSeed;

	EGunTraceHit GunTraceHitCache;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 ClipSize;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bFiring : 1;
	uint8 bIsGunTraceCacheValid : 1;
};
