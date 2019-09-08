// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"

#include "Saucewich.h"
#include "Interface/Colorable.h"
#include "Interface/Translucentable.h"

#include "TpsCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupStarted, float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupCanceled);

USTRUCT()
struct FPerkInstance
{
	GENERATED_BODY()
	
	FTimerHandle Timer;
	class UParticleSystemComponent* PSC;
};

UCLASS(Abstract)
class SAUCEWICH_API ATpsCharacter : public ACharacter, public IColorable, public ITranslucentable
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

	// 플레이어가 현재 들고있는 무기로 Trace를 실시합니다.
	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit) const;

	UFUNCTION(BlueprintCallable)
	uint8 GetTeam() const;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColor() const;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetTeamColor() const;

	void SetColor(const FLinearColor& NewColor) override;

	bool IsAlive() const { return bAlive; }
	bool IsInvincible() const;
	void SetMaxHP(float Ratio);

	/**
	 * 캐릭터의 이동속도 비율을 반환합니다. 기본값은 WeaponComponent::GetSpeedRatio 입니다.
	 * @return 캐릭터 이동속도 비율 (0~1)
	 */
	UFUNCTION(BlueprintNativeEvent)
	float GetSpeedRatio() const;

	UFUNCTION(BlueprintCallable)
	void AddPerk(TSubclassOf<class APerk> PerkClass);

	UFUNCTION(BlueprintCallable)
	bool HasPerk(TSubclassOf<APerk> PerkClass) const;

	// 주의: Simulated Proxy에서는 추가 계산이 들어갑니다.
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
	void PostInitializeComponents() override;
	void SetupPlayerInputComponent(class UInputComponent* Input) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	bool ShouldTakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

	// 캐릭터를 죽입니다.
	void Kill(class ASaucewichPlayerState* Attacker = nullptr, AActor* Inflictor = nullptr);

	// 캐릭터를 살립니다.
	void SetPlayerDefaults() override;

	/**
	 * 캐릭터의 현재 방어력을 구합니다.
	 * @return 0~1 사이의 값으로, 1이면 무적입니다. 기본값은 0입니다.
	 * @warning Server/Client 모두에게서 값이 같아야합니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	float GetArmor() const;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void SetActorActivated(bool bActive);

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION()
	void OnTeamChanged(uint8 NewTeam);
	void BindOnTeamChanged();
	void SetColorToTeamColor();
	FLinearColor GetTeamColor(class ASaucewichGameState* GameState) const;
	int32 GetColIdx() const;

	UFUNCTION()
	void OnRep_Alive();

	void BeTranslucent() override;
	void BeOpaque() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddPerk(UClass* PerkClass);

	UPROPERTY(Transient, VisibleInstanceOnly)
	TMap<TSubclassOf<APerk>, FPerkInstance> Perks;

	FTimerHandle RespawnInvincibleTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	const class UCharacterData* Data;

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* ColMat;

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* ColTranslMat;

	// 현재 최대 체력입니다. 장착중인 주무기 등 여러 요인에 의해 달라집니다.
	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float MaxHP;

	// 현재 체력입니다.
	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float HP;

	UPROPERTY(ReplicatedUsing=OnRep_Alive, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bAlive : 1;
	uint8 bTranslucent : 1;
};
