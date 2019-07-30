// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Colorable.h"
#include "Weapon.generated.h"

UCLASS(Abstract)
class AWeapon : public AActor, public IColorable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* Mesh;
	
public:	
	AWeapon();

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	bool IsEquipped() const { return bEquipped; }
	uint8 GetSlot() const { return Slot; }

	bool IsVisible() const;
	void SetVisibility(bool bNewVisibility) const;

	FLinearColor GetColor() const;
	void SetColor(const FLinearColor& NewColor) override;

	virtual void FireP() {}
	virtual void FireR() {}
	virtual void SlotP() {}
	virtual void SlotR() {}

	virtual void Deploy();
	virtual void Holster();
	virtual bool CanDeploy() const { return true; }
	virtual bool CanHolster() const { return true; }

	class ATpsCharacter* GetCharacter() const;

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Init();

	UFUNCTION()
	virtual void OnRep_Equipped();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	ATpsCharacter* Owner;
	UMaterialInstanceDynamic* Material;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 TeamColorMaterialElementIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Slot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Equipped, Transient, meta=(AllowPrivateAccess=true))
	uint8 bEquipped : 1;
};
