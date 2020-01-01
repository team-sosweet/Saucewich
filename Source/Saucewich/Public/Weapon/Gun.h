// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Weapon/Weapon.h"
#include "Gun.generated.h"

USTRUCT(BlueprintType)
struct SAUCEWICH_API FGunData : public FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<class USoundBase> FireSound;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class UCameraShake> FireShake;

	// 자동조준 상자 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D TraceBoxSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TSoftClassPtr<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TSoftClassPtr<class AGunProjectile> ProjectileClass;

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

	// 초탄퍼짐 비율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	float FirstSpreadRatio = .2f;
	
	// 발사당 탄퍼짐 증가 비율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, UIMax=1, ClampMin=0, ClampMax=1))
	float SpreadIncrease = .2f;

	// 초당 탄퍼짐 감소율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, ClampMin=0))
	float SpreadDecrease = 1.f;

	// 지향사격 각도 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, UIMax=60))
	float HipFireAngleOffset = 25.f;

	// 자동 조준 최대 거리 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistance = 1000.f;

	// 발사되는 소스 발사체의 속력 (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProjectileSpeed = 1000.f;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, ClampMin=0))
	float Recoil = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = 0, UIMax=1, ClampMin=0, ClampMax=1))
	float FBBIntensity = .05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = 0, ClampMin = 0))
	float FBBDuration = .2f;

	// 탄창 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 ClipSize;

	// 소스를 완전히 소모하고 재장전중에 소스가 이만큼 차면 다시 발사가 가능해집니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MinClipToFireAfterDried;

	// 한 번에 발사되는 탄자 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=1, ClampMin=1))
	uint8 NumProjectile = 1;
};

UCLASS(Abstract)
class SAUCEWICH_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();
	
	UFUNCTION(BlueprintCallable)
	bool GunTrace(FHitResult& OutHit);

	UFUNCTION(BlueprintCallable)
	const FGunData& GetGunData() const;

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

	void SetColor(const FLinearColor& NewColor) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnShoot();

private:
	void Shoot();
	void StartFire(int32 RandSeed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire(int32 RandSeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartFire(int32 RandSeed);

	bool GunTraceInternal(FHitResult& OutHit, FName ProjColProf, const FGunData& Data);

	UFUNCTION()
	void OnRep_Dried() const;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* FirePSC;

	FRandomStream FireRand;
	float SpreadAlpha;

	float FireLag;

	float ReloadWaitingTime;
	float ReloadAlpha;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;
	uint8 LastClip;

	UPROPERTY(ReplicatedUsing=OnRep_Dried, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bDried : 1;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bFiring : 1;

	UPROPERTY(Replicated, Transient)
	uint8 bFreeze : 1;
};
