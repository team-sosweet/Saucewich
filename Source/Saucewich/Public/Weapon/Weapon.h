// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "PoolActor.h"

#include "Engine/DataTable.h"

#include "Interface/Colorable.h"
#include "Interface/Translucentable.h"

#include "Weapon.generated.h"

class UTexture;

DECLARE_MULTICAST_DELEGATE(FOnColMatCreated);

USTRUCT(BlueprintType)
struct SAUCEWICH_API FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture> Icon_Team;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture> Icon_Rest;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WalkSpeedRatio = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HPRatio = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Slot = 0;
};

/**
 * 모든 무기의 기본이 되는 클래스입니다.
 * 이름과는 달리 '캐릭터가 지니고 있을 수 있으며 슬롯을 누르면 특정 행동을 할 수 있는' 그 어떤 것도 될 수 있습니다.
 */
UCLASS(Abstract)
class SAUCEWICH_API AWeapon : public APoolActor, public IColorable, public ITranslucentable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;
public:	
	AWeapon();

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	bool IsEquipped() const { return bEquipped; }

	template <class T = FWeaponData>
	const T* GetData(const TCHAR* const ContextString) const
	{
		static_assert(TIsDerivedFrom<T, FWeaponData>::IsDerived, "");
		return WeaponData.GetRow<T>(ContextString);
	}

	// 무기 데이터에 대한 레퍼런스를 반환합니다.
	// 만약 무기 클래스에 데이터가 바인드 되어있지 않거나 하는 이유로 데이터를 구할 수 없을 경우 기본값을 반환합니다.
	UFUNCTION(BlueprintCallable)
	const FWeaponData& GetWeaponData() const;

	template <class T>
	const T* GetSharedData() const
	{
		static_assert(TIsDerivedFrom<T, class UWeaponSharedData>::IsDerived, "");
		return Cast<T>(SharedData);
	}
	auto GetSharedData() const { return SharedData; }

	bool IsVisible() const;
	void SetVisibility(bool bNewVisibility) const;

	FLinearColor GetColor() const;
	void SetColor(const FLinearColor& NewColor) override;

	void BeTranslucent() override;
	void BeOpaque() override;

	// [Shared] 키를 누르거나 뗄 때 호출됩니다.
	virtual void FireP() {}
	virtual void FireR() {}
	virtual void SlotP() {}
	virtual void SlotR() {}

	// [Shared] 무기를 장비하면 호출됩니다.
	virtual void Deploy();

	// [Shared] 무기를 집어넣으면 호출됩니다.
	virtual void Holster();

	virtual bool CanDeploy() const { return IsActive(); }
	virtual bool CanHolster() const { return true; }

protected:
	void PostInitializeComponents() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnActivated() override;
	void OnReleased() override;

private:
	void Init();

	UFUNCTION()
	virtual void OnRep_Equipped();

	int32 GetColIdx() const;

	FOnColMatCreated OnColMatCreated;

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle WeaponData;

	UPROPERTY(EditAnywhere)
	const class UWeaponSharedData* SharedData;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* ColMat;
	
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* ColTranslMat;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Equipped, Transient, meta=(AllowPrivateAccess=true))
	uint8 bEquipped : 1;
	uint8 bTransl : 1;
};
