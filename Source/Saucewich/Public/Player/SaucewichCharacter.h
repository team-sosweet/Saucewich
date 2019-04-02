// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SaucewichCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTickDelegate, float)

UENUM()
enum class EDirection : uint8
{
	Left, Right
};

UCLASS(Abstract)
class ASaucewichCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ASaucewichCharacter();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void GiveWeapon(class AWeapon* Weapon);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* TurnAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float TurnAnimRate = 90.f;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	float BaseTurnRate = 45.f;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	float BaseLookUpRate = 45.f;

	UPROPERTY(VisibleInstanceOnly, Transient)
	class AWeapon* Weapon;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	FTickDelegate PostTick;

	//////////////////////////////////////////////////////////////////////////
	// Turn when not moving

	void TurnWhenNotMoving();
	bool CheckShouldTurn(EDirection& OutDirection);
	void StartTurn(EDirection Direction);
	void StartTurn_Internal(EDirection Direction);
	void SimulateTurn(EDirection Direction);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartTurn(EDirection Direction);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSimulateTurn(EDirection Direction);

	FDelegateHandle DoTurn;
	float TurnAlpha;

	//////////////////////////////////////////////////////////////////////////
	// Replication

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FRotator GetBaseAimRotation() const override;

	void ReplicateCameraYaw();

	UPROPERTY(Replicated, Transient)
	uint8 RemoteViewYaw;

	//////////////////////////////////////////////////////////////////////////
	// Input

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
};
