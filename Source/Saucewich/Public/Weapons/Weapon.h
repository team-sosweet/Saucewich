// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponEnum.h"
#include "Weapon.generated.h"

UCLASS(Abstract)
class SAUCEWICH_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void StartAttack() {};
	virtual void StopAttack() {};
	virtual bool CanAttack() const;
	virtual void Deploy() {};
	virtual void Holster() {};

	void SetColor(const FLinearColor& Color);
	FLinearColor GetColor() const;

	auto GetPosition() const { return Position; }
	auto GetWeight() const { return Weight; }
	auto GetGripType() const { return GripType; }

protected:
	virtual void PostInitializeComponents() override;

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly)
	EWeaponPosition Position;

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	EWeaponGripType GripType;

	void SetActivated(bool bActive);

	class UMaterialInstanceDynamic* ColorDynamicMaterial;
};
