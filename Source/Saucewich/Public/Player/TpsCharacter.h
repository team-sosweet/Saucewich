// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "TpsCharacter.generated.h"

UCLASS()
class ATpsCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UWeaponComponent* WeaponComponent;

public:
	ATpsCharacter();

	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }
	UWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

protected:
	void SetupPlayerInputComponent(class UInputComponent* Input) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
};
