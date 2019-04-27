// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

UCLASS()
class SAUCEWICH_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartAttack() override;
	virtual void StopAttack() override;
	virtual bool CanAttack() const override;

private:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Muzzle;

	UPROPERTY(VisibleAnywhere)
	class UActorPoolComponent* ProjPool;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Clip;

	UPROPERTY(EditAnywhere)
	int32 NumProjFiredAtOnce{ 1 };

	UPROPERTY(EditAnywhere)
	uint8 bFullAuto : 1;

	UPROPERTY(EditAnywhere)
	float AttackDelay;

	UPROPERTY(EditAnywhere)
	int32 MinClipToShootAfterDried;

	UPROPERTY(EditAnywhere)
	float ReloadTime;

	UPROPERTY(EditAnywhere)
	float ReloadWaitTime;

	UPROPERTY(EditAnywhere)
	float ProjSpeed;

	int32 LastClip;
	float ReloadAlpha;
	UPROPERTY(VisibleInstanceOnly, Replicated, Transient)
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

	const auto* GetCDO() const { return GetClass()->GetDefaultObject<AGun>(); }
};
