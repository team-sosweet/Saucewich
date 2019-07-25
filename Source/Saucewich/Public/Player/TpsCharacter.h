// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "GameFramework/Character.h"
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

UCLASS()
class ATpsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATpsCharacter();

	class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	class UCameraComponent* GetCamera() const { return Camera; }
	class UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	class UStaticMeshComponent* GetShadow() const { return Shadow; }

	float GetSpringArmLength() const;

	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit) const;

	FVector GetPawnViewLocation() const override;

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void SetupPlayerInputComponent(class UInputComponent* Input) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void UpdateShadow() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* Shadow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FShadowData ShadowData;
};
