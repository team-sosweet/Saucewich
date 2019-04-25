// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

USTRUCT(BlueprintType)
struct FGunData : public FWeaponData
{
	GENERATED_BODY()

	virtual UClass* GetBaseClass() const override;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	int32 NumberOfProjectilesFiredAtOnce{ 1 };

	UPROPERTY(EditAnywhere)
	uint8 bFullAuto : 1;

	UPROPERTY(EditAnywhere)
	float AttackDelay;

	UPROPERTY(EditAnywhere)
	int32 SauceAmount;

	UPROPERTY(EditAnywhere)
	int32 MinSauceAmountToShootWhenFullReload;

	UPROPERTY(EditAnywhere)
	float ReloadTime;

	UPROPERTY(EditAnywhere)
	float ReloadWaitTime;

	UPROPERTY(EditAnywhere)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere)
	float ProjectileScale{ 1.f };

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APoolActor> ProjectileClass;
};

UCLASS()
class SAUCEWICH_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();
	auto GetData() const { return static_cast<const FGunData*>(Super::GetData()); }

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Muzzle;

	UPROPERTY(VisibleAnywhere)
	class UActorPoolComponent* ProjectilePool;

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartAttack() override;
	virtual void StopAttack() override;
	virtual bool CanAttack() const override;
	virtual void Equip(const FWeaponData* NewWeaponData) override;

	UPROPERTY(EditInstanceOnly, Replicated, Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 SauceAmount;
	int32 LastSauceAmount;
	float ReloadAlpha;
	float ReloadWaitTime;
	uint8 bDried : 1;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Attacking, Transient, meta = (AllowPrivateAccess = true))
	uint8 bAttacking : 1;
	uint8 bOldAttacking : 1;

	UFUNCTION()
	void OnRep_Attacking();

	float NextAttackTime;

	void HandleAttack();
	AActor* ShootSauce();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSingleAttack();

	void Reload(float DeltaTime);
};
