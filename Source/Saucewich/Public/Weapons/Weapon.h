// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponGripType : uint8
{
	Rifle, Pistol
};

UCLASS(Abstract)
class SAUCEWICH_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Muzzle;

	UPROPERTY(VisibleAnywhere)
	class UActorPoolComponent* ProjectilePool;

	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditAnywhere, Replicated, Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 SauceAmount;

	UPROPERTY(EditAnywhere)
	uint8 bFullAuto : 1;

	UPROPERTY(EditAnywhere)
	float AttackDelay = .1f;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	float MuzzleSpeed;

	UPROPERTY(EditAnywhere)
	int32 NumberOfProjectilesFiredAtOnce = 1;

public:
	UPROPERTY(EditAnywhere)
	EWeaponGripType GripType;

	void Attack();
	void StopAttack();
	bool CanAttack() const;

private:
	void SetActivated(bool bActive);

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
};
