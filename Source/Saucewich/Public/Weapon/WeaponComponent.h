// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "Components/SceneComponent.h"
#include "Colorable.h"
#include "Translucentable.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, class AWeapon*, Weapon);

/**
 * 캐릭터와 무기가 상호작용하는 중간다리입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent : public USceneComponent, public IColorable, public ITranslucentable
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void SetupPlayerInputComponent(class UInputComponent* Input);

	/**
	 * [Server] Gives a weapon. Replaces if already in the same slot.
	 * @param WeaponClass: Class of weapon to give
	 * @return: The weapon given
	 */
	UFUNCTION(BlueprintCallable)
	virtual AWeapon* Give(TSubclassOf<AWeapon> WeaponClass);

	/**
	 * [Shared] Returns active weapon.
	 * @return: The weapon currently equipped
	 */
	UFUNCTION(BlueprintCallable)
	AWeapon* GetActiveWeapon() const { return GetWeapon(Active); }

	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeapon(const uint8 Slot) const { return Slot < Weapons.Num() ? Weapons[Slot] : nullptr; }

	UFUNCTION(BlueprintCallable)
	EGunTraceHit GunTrace(FHitResult& OutHit) const;

	void OnCharacterDeath();
	virtual bool TrySelectWeapon(uint8 Slot);

	void BeTranslucent() override;
	void BeOpaque() override;
	void SetColor(const FLinearColor& NewColor) override;

	float GetSpeedRatio() const;
	uint8 GetSlots() const { return WeaponSlots; }

	/**
	 * 새 무기를 습득했을 때 발동되는 델리게이트에 새 함수를 추가합니다.
	 * @param UserObject - 함수를 호출할 객체
	 * @param FuncName - 함수 이름 (예: &AActor::Destroy)
	 * @param bIncludeCurrentWeapons - 이미 장비중인 무기에 대해서 바로 호출되길 원하면 true
	 */
#define AddOnEquipWeapon(UserObject, FuncName, bIncludeCurrentWeapons) __Internal_AddOnEquipWeapon(UserObject, FuncName, STATIC_FUNCTION_FNAME(TEXT(#FuncName)), bIncludeCurrentWeapons)

	class ATpsCharacter* const Owner = nullptr;

protected:
	void InitializeComponent() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_Weapons();

private:
	UFUNCTION(BlueprintCallable) void FireP();
	UFUNCTION(BlueprintCallable) void FireR();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireR();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireP();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireR();

	UFUNCTION(BlueprintCallable) void SlotP(uint8 Slot);
	UFUNCTION(BlueprintCallable) void SlotR(uint8 Slot);
	UFUNCTION(Server, Reliable, WithValidation) void ServerSlotP(uint8 Slot);
	UFUNCTION(Server, Reliable, WithValidation) void ServerSlotR(uint8 Slot);
	UFUNCTION(NetMulticast, Reliable) void MulticastSlotP(uint8 Slot);
	UFUNCTION(NetMulticast, Reliable) void MulticastSlotR(uint8 Slot);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(AWeapon* Weapon);

	FOnEquipWeapon OnEquipWeapon;

	// 현재 캐릭터가 가지고 있는 무기 목록입니다.
	UPROPERTY(ReplicatedUsing=OnRep_Weapons, VisibleInstanceOnly, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<AWeapon*> Weapons;

	// 무기 슬롯 개수입니다. 각 슬롯은 키보드 숫자 1 ~ 9 키에 대응됩니다.
	UPROPERTY(EditDefaultsOnly, meta=(UIMax=9, ClampMax=9), BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 WeaponSlots;

	// 현재 활성화된 무기 슬롯 index 입니다.
	UPROPERTY(VisibleInstanceOnly, Replicated, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Active;

public:
	// 주의: 이 함수를 직접 호출하지 마십시오. 대신 AddOnEquipWeapon 매크로를 사용하시기 바랍니다.
	template <class UserClass>
	void __Internal_AddOnEquipWeapon(UserClass* const UserObject, const typename TBaseDynamicDelegate<FWeakObjectPtr, void, AWeapon*>::TMethodPtrResolver<UserClass>::FMethodPtr MethodPtr, FName FunctionName, const bool bIncludeCurrentWeapons)
	{
		if (bIncludeCurrentWeapons)
			for (const auto Weapon : Weapons)
				if (Weapon) (UserObject->*MethodPtr)(Weapon);
		OnEquipWeapon.__Internal_AddDynamic(UserObject, MethodPtr, FunctionName);
	}
};
