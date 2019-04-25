// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "WeaponEnum.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	virtual UClass* GetBaseClass() const;

	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere)
	EWeaponPosition Position;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	EWeaponGripType GripType;
};

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
	virtual void Equip(const FWeaponData* NewWeaponData);

	UFUNCTION(BlueprintCallable)
	void SetColor(const FLinearColor& Color);

	auto GetData() const { return DataTable; }

protected:
	virtual void BeginPlay() override;
	auto GetMesh() const { return Mesh; }

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	class UMaterialInstanceDynamic* ColorDynamicMaterial;

	const FWeaponData* DataTable;

	void SetActivated(bool bActive);
};
