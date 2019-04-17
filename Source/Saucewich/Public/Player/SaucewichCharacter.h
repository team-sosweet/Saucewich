// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SaucewichCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTickDelegate, float)
DECLARE_EVENT(ASaucewichCharacter, FOnCharacterDeath)

UENUM()
enum class EDirection : uint8
{
	Left, Right
};

class AWeapon;

UCLASS(Abstract, Config = Input)
class ASaucewichCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ASaucewichCharacter();

	FOnCharacterDeath OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void GiveWeapon(TSubclassOf<AWeapon> WeaponClass);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeapon() const { return Weapon; }

	bool CanAttack() const;

	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetBaseAimRotation() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* TurnAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float TurnAnimRate = 90.f;

	UPROPERTY(EditAnywhere, Category = "Camera", Config)
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, Category = "Camera", Config)
	float BaseLookUpRate = 45.f;

	virtual void Tick(float DeltaTime) override;
	FTickDelegate PostTick;

	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnHPChanged, meta = (AllowPrivateAccess = true))
	float HP = 100.f;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnHPChanged();

	void Kill();

	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AWeapon* Weapon;

	void WeaponAttack();
	void WeaponStopAttack();

	//////////////////////////////////////////////////////////////////////////

	void TurnWhenNotMoving();
	bool CheckShouldTurn(EDirection& OutDirection);
	void StartTurn(EDirection Direction);
	void StartTurn_Internal(EDirection Direction);
	void PlayTurnAnim(EDirection Direction);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartTurn(EDirection Direction);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSimulateTurn(EDirection Direction);

	FDelegateHandle DoTurn;
	float TurnAlpha;

	//////////////////////////////////////////////////////////////////////////

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ReplicateView();

	UPROPERTY(Replicated, Transient)
	uint8 RemoteViewYaw;

	UPROPERTY(Replicated, Transient)
	FVector_NetQuantize RemoteViewLocation;

	//////////////////////////////////////////////////////////////////////////

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
};
