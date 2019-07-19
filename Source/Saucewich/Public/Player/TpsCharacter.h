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

public:
	ATpsCharacter();

	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }

protected:
	void SetupPlayerInputComponent(class UInputComponent* Input) override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
};
