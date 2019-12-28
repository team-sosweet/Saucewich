// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Saucewich.h"
#include "TpsCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupStarted, float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float, NewHP);

USTRUCT()
struct FPerkInstance
{
	GENERATED_BODY()
	
	FTimerHandle Timer;
	class UParticleSystemComponent* PSC;
};

UCLASS(Abstract)
class SAUCEWICH_API ATpsCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UShadowComponent* Shadow;

public:
	explicit ATpsCharacter(const FObjectInitializer& ObjectInitializer);

	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }
	UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

	class AWeapon* GetActiveWeapon() const;

	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit) const;

	UFUNCTION(BlueprintCallable)
	uint8 GetTeam() const;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColor() const;

	UFUNCTION(BlueprintCallable)
	const FLinearColor& GetTeamColor() const;

	void SetColor(const FLinearColor& NewColor) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Kill(class ASaucewichPlayerState* Attacker = nullptr, AActor* Inflictor = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void KillSilent();

	bool IsAlive() const { return bAlive; }
	bool IsInvincible() const;

	UFUNCTION(BlueprintNativeEvent)
	float GetSpeedRatio() const;

	UFUNCTION(BlueprintCallable)
	void AddPerk(TSubclassOf<class APerk> PerkClass);

	UFUNCTION(BlueprintCallable)
	bool HasPerk(TSubclassOf<APerk> PerkClass) const;

	FVector GetPawnViewLocation() const override;
	FRotator GetBaseAimRotation() const override { return Super::GetBaseAimRotation().GetNormalized(); }
	FVector GetSpringArmLocation() const;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterSpawn OnCharacterSpawn;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeath OnCharacterDeath;

	UPROPERTY(BlueprintAssignable)
	FOnPickupStarted OnPickupStarted;

	UPROPERTY(BlueprintAssignable)
	FOnPickupCanceled OnPickupCanceled;

protected:
	void BeginPlay() override;
	void Destroyed() override;
	void PostInitializeComponents() override;
	
	void PossessedBy(AController* NewController) override;
	void OnRep_Controller() override;
	virtual void OnControllerChanged();
	
	void SetupPlayerInputComponent(class UInputComponent* Input) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	bool ShouldTakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnKilled();

	void SetPlayerDefaults() override;

	UFUNCTION(BlueprintNativeEvent)
	float GetArmorRatio() const;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void SetActorActivated(bool bActive);

	UFUNCTION()
	void OnTeamChanged(uint8 NewTeam);
	void BindOnTeamChanged();
	int32 GetColIdx() const;

	UFUNCTION()
	void OnRep_Alive();

	UFUNCTION()
	void OnRep_HP() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastKill(ASaucewichPlayerState* Attacker, AActor* Inflictor);
	void Kill_Internal(ASaucewichPlayerState* Attacker, AActor* Inflictor);
	void SpawnDeathEffects() const;

	void BeTranslucent();
	void BeOpaque();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddPerk(UClass* PerkClass);

	UPROPERTY(Transient, VisibleInstanceOnly)
	TMap<TSubclassOf<APerk>, FPerkInstance> Perks;

	UPROPERTY(BlueprintAssignable)
	FOnHPChanged OnHPChanged;

	FTimerHandle RespawnInvincibleTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	const class UCharacterData* Data;

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* ColMat;

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* ColTranslMat;

	UPROPERTY(ReplicatedUsing=OnRep_HP, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float HP;

	UPROPERTY(ReplicatedUsing=OnRep_Alive, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bAlive : 1;
	uint8 bTranslucent : 1;
};
