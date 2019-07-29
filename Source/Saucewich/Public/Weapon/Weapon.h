// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS(Abstract)
class AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* Mesh;
	
public:	
	AWeapon();

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void FireP() {}
	virtual void FireR() {}
	virtual void SlotP() {}
	virtual void SlotR() {}

	virtual void Deploy();
	virtual void Holster();
	virtual bool CanDeploy() const { return true; }
	virtual bool CanHolster() const { return true; }

	bool IsEquipped() const { return bEquipped; }
	uint8 GetSlot() const { return Slot; }
	UStaticMeshComponent* GetMesh() const { return Mesh; }
	bool IsVisible() const;
	void SetVisibility(bool bNewVisibility) const;
	class ATpsCharacter* GetCharacter() const;

private:
	void Init();

	UFUNCTION()
	virtual void OnRep_Equipped();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 Slot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Equipped, Transient, meta=(AllowPrivateAccess=true))
	uint8 bEquipped : 1;

	ATpsCharacter* Owner;
};
