// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "WeaponEnum.h"
#include "SaucewichCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTickDelegate, float)

UENUM()
enum class EDirection : uint8
{
	Left, Right
};

UCLASS(Abstract, Config = Input)
class ASaucewichCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ASaucewichCharacter();

	auto Alive() const { return HP > 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void GiveWeapon(TSubclassOf<AWeapon> WeaponClass);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetActiveWeapon() const { return Weapon[ActiveWeaponIdx]; }

	UFUNCTION(BlueprintCallable)
	void SetColor(const FLinearColor& Color);
	FLinearColor GetColor() const;

	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetBaseAimRotation() const override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* TurnAnim;

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	FTickDelegate PostTick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnHPChanged, meta = (AllowPrivateAccess = true))
	float HP{ 100.f };

	UFUNCTION()
	void OnHPChanged();

	void Kill();

	UPROPERTY(Replicated, Transient)
	AWeapon* Weapon[static_cast<uint8>(EWeaponPosition::_MAX)];

	UPROPERTY(Replicated, Transient)
	uint8 ActiveWeaponIdx;

	void WeaponAttack();
	void WeaponStopAttack();

	class UMaterialInstanceDynamic* ClothColorDynamicMaterial;

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ReplicateView();

	UPROPERTY(Replicated, Transient)
	uint8 RemoteViewYaw;

	UPROPERTY(Replicated, Transient)
	FVector_NetQuantize RemoteViewLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float TurnAnimRate{ 90.f };

	UPROPERTY(EditAnywhere, Category = "Camera", Config)
	float BaseTurnRate{ 45.f };

	UPROPERTY(EditAnywhere, Category = "Camera", Config)
	float BaseLookUpRate{ 45.f };

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
};
