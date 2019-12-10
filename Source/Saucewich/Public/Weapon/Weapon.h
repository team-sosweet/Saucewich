// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

class UTexture;
class UWeaponSharedData;

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
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, ClampMin=0))
	float WalkSpeedRatio = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0, ClampMin=0))
	float ArmorRatio = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 Slot;
};

/**
 * 모든 무기의 기본이 되는 클래스입니다.
 * 이름과는 달리 '캐릭터가 지니고 있을 수 있으며 슬롯을 누르면 특정 행동을 할 수 있는' 그 어떤 것도 될 수 있습니다.
 */
UCLASS(Abstract)
class SAUCEWICH_API AWeapon : public APoolActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;
	
public:
	UFUNCTION(BlueprintCallable)
	static AWeapon* GetDefaultWeapon(TSubclassOf<AWeapon> Class);
	
	AWeapon();

	UStaticMeshComponent* GetMesh() const { return Mesh; }
	bool IsEquipped() const { return bEquipped; }

	template <class T = FWeaponData>
	const T& GetData() const
	{
		static_assert(TIsDerivedFrom<T, FWeaponData>::IsDerived, "T must be derived from FWeaponData");
		static const T Default{};
		const auto Data = WeaponData.GetRow<T>(TEXT(""));
		return ensure(Data) ? *Data : Default;
	}

	UFUNCTION(BlueprintCallable)
	const FWeaponData& GetWeaponData() const;

	template <class T>
	const T& GetSharedData() const
	{
		static_assert(TIsDerivedFrom<T, UWeaponSharedData>::IsDerived, "T must be derived from UWeaponSharedData");
		const auto Data = Cast<T>(&GetSharedData());
		return ensure(Data) ? *Data : *GetDefault<T>(T::StaticClass());
	}
	const UWeaponSharedData& GetSharedData() const;

	bool IsVisible() const;
	void SetVisibility(bool bNewVisibility) const;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColor() const;
	virtual void SetColor(const FLinearColor& NewColor);

	void BeTranslucent();
	void BeOpaque();

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
