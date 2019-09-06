// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "Components/SceneComponent.h"
#include "Colorable.h"
#include "Translucentable.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, class AWeapon*, Weapon);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEquipWeaponSingle, class AWeapon*, Weapon);

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

	UFUNCTION(BlueprintCallable)
	void AddOnEquipWeapon(const FOnEquipWeaponSingle& Delegate, bool bCallbackWithCurrentWeapons = true);

	struct FBroadcastEquipWeapon
	{
	private:
		friend AWeapon;
		
		FBroadcastEquipWeapon(UWeaponComponent* WeaponComponent, AWeapon* Weapon)
		{
			WeaponComponent->OnEquipWeapon.Broadcast(Weapon);
		}
	};

protected:
	void InitializeComponent() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_Weapons();

private:
	UFUNCTION(BlueprintCallable) void FireP();
	UFUNCTION(BlueprintCallable) void FireR();
	void StartFire();
	void StopFire();
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

	uint8 bFirePressed : 1;
	uint8 bShouldAutoFire : 1;
	uint8 bFiring : 1;
};
