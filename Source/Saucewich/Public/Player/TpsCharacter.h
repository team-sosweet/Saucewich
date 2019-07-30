// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "GameFramework/Character.h"
#include "Colorable.h"
#include "TpsCharacter.generated.h"

USTRUCT(BlueprintType)
struct FShadowData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Darkness = .9f;

	class UMaterialInstanceDynamic* Material;
};

UCLASS(Abstract)
class ATpsCharacter : public ACharacter, public IColorable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Shadow;

public:
	explicit ATpsCharacter(const FObjectInitializer& ObjectInitializer);

	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }
	UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	UStaticMeshComponent* GetShadow() const { return Shadow; }

	class AWeapon* GetActiveWeapon() const;

	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit) const;

	UFUNCTION(BlueprintCallable)
	uint8 GetTeam() const;

	FLinearColor GetColor() const;
	FLinearColor GetTeamColor() const;
	void SetColor(const FLinearColor& NewColor) override;

	void SetMaxHP(float Ratio);
	virtual float GetSpeedRatio() const;

	// 주의: Simulated Proxy에서는 추가 계산이 들어갑니다.
	FVector GetPawnViewLocation() const override;
	FRotator GetBaseAimRotation() const override { return Super::GetBaseAimRotation().GetNormalized(); }
	FVector GetSpringArmLocation() const;

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void PostInitializeComponents() override;
	void SetupPlayerInputComponent(class UInputComponent* Input) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	bool ShouldTakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

	// 캐릭터가 사망하여 소멸되기 직전에 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent)
	void OnKill();
	virtual void Kill() { OnKill(); Destroy(); }

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	UFUNCTION()
	void OnTeamChanged(uint8 NewTeam);
	void BindOnTeamChanged();

	void UpdateShadow() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FShadowData ShadowData;

	UMaterialInstanceDynamic* Material;

	UPROPERTY(EditDefaultsOnly)
	float DefaultMaxHP;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float MaxHP;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float HP;
};
