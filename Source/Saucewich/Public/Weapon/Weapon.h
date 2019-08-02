// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Colorable.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponIcon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UTexture* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture* IconMask;
};

/*
 * 모든 무기의 기본이 되는 클래스입니다.
 * 이름과는 달리 '캐릭터가 지니고 있을 수 있으며 슬롯을 누르면 특정 행동을 할 수 있는' 그 어떤 것도 될 수 있습니다.
 */
UCLASS(Abstract)
class AWeapon : public AActor, public IColorable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;
	
public:	
	AWeapon();

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	bool IsEquipped() const { return bEquipped; }
	uint8 GetSlot() const { return Slot; }

	float GetSpeedRatio() const { return WalkSpeedRatio; }
	float GetHPRatio() const { return HPRatio; }

	bool IsVisible() const;
	void SetVisibility(bool bNewVisibility) const;

	FLinearColor GetColor() const;
	void SetColor(const FLinearColor& NewColor) override;

	// [Shared] 키를 누르거나 뗄 때 호출됩니다.
	virtual void FireP() {}
	virtual void FireR() {}
	virtual void SlotP() {}
	virtual void SlotR() {}

	// [Shared] 무기를 장비하면 호출됩니다.
	virtual void Deploy();

	// [Shared] 무기를 집어넣으면 호출됩니다.
	virtual void Holster();

	virtual bool CanDeploy() const { return true; }
	virtual bool CanHolster() const { return true; }

	class ATpsCharacter* GetCharacter() const;

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void PostInitializeComponents() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Init();

	UFUNCTION()
	virtual void OnRep_Equipped();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FWeaponIcon Icon;

	ATpsCharacter* Owner;
	UMaterialInstanceDynamic* Material;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float WalkSpeedRatio = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float HPRatio = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Slot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Equipped, Transient, meta=(AllowPrivateAccess=true))
	uint8 bEquipped : 1;
};
