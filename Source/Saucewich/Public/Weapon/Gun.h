// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "Weapon/Weapon.h"
#include "Engine/CollisionProfile.h"
#include "Gun.generated.h"

USTRUCT(BlueprintType)
struct SAUCEWICH_API FGunData : public FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FCollisionProfileName PawnOnly;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FCollisionProfileName NoPawn;

	// 자동조준 상자 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D TraceBoxSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TSubclassOf<class AGunProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	float TraceStartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage;

	// 분당 발사 수 (연사력)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Rpm;

	// 소스 발사 수직 퍼짐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	float VerticalSpread;

	// 소스 발사 수평 퍼짐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	float HorizontalSpread;

	// 자동 조준 최대 거리 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistance;

	// 발사되는 소스 발사체의 속력 (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProjectileSpeed;

	// 최소 발사체 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinProjectileSize = 1.f;

	// 최대 발사체 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxProjectileSize = 1.f;

	// 재장전에 걸리는 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadTime;

	// 재장전 시작까지 걸리는 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadWaitTime;

	// 소스를 완전히 소모하고 재장전 시작까지 걸리는 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadWaitTimeAfterDried;

	// 탄창 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 ClipSize;

	// 소스를 완전히 소모하고 재장전중에 소스가 이만큼 차면 다시 발사가 가능해집니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MinClipToFireAfterDried;
};

UCLASS(Abstract)
class SAUCEWICH_API AGun final : public AWeapon
{
	GENERATED_BODY()

public:
	void Shoot();

	// 이 총으로 target을 찾는 trace를 수행합니다.
	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit) const;

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CanFire() const;
	void Reload(float DeltaSeconds);
	void FireP() override;
	void FireR() override;
	void SlotP() override;

	void OnActivated() override;
	void OnReleased() override;

private:
	UFUNCTION()
	void OnRep_FireRandSeed();

	FRandomStream FireRand;

	float FireLag;
	float LastFire;

	float ReloadWaitingTime;
	float ReloadAlpha;

	UPROPERTY(ReplicatedUsing=OnRep_FireRandSeed, Transient)
	int32 FireRandSeed;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;
	uint8 LastClip;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bDried : 1;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bFiring : 1;
};
