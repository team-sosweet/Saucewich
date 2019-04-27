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

public:
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

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> BaseClass;
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
	virtual void Equip(const FWeaponData* NewWeaponData, const FName& NewDataTableRowName);

	void SetColor(const FLinearColor& Color);
	FLinearColor GetColor() const;

	auto GetData() const { return WeaponData; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	const UDataTable* DataTable;

	const FWeaponData* WeaponData;

	UPROPERTY(ReplicatedUsing = OnRep_DataTableRowName, Transient)
	FName DataTableRowName;

	UFUNCTION()
	void OnRep_DataTableRowName();

	void SetActivated(bool bActive);

	class UMaterialInstanceDynamic* ColorDynamicMaterial;
};
