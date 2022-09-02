// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Saucewich.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipWeapon, class AWeapon*, Weapon);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEquipWeaponSingle, class AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWepAvailabilityChanged, bool, bAvailable);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWepAvailabilityChangedSingle, bool, bAvailable);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	void SetupPlayerInputComponent(UInputComponent* Input);

	UFUNCTION(BlueprintCallable)
	AWeapon* Give(const TSoftClassPtr<AWeapon>& WeaponClass);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetActiveWeapon() const { return GetWeapon(Active); }

	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeapon(const uint8 Slot) const { return Slot < Weapons.Num() ? Weapons[Slot] : nullptr; }

	UFUNCTION(BlueprintCallable)
	bool GunTrace(FHitResult& OutHit) const;

	void OnCharacterDeath();
	bool TrySelectWeapon(uint8 Slot);

	void SetColor(const FLinearColor& NewColor);

	float GetSpeedRatio() const;
	float GetArmorRatio() const;
	uint8 GetSlots() const { return WeaponSlots; }

	UFUNCTION(BlueprintCallable)
	void AddOnEquipWeapon(const FOnEquipWeaponSingle& Delegate);

	UFUNCTION(BlueprintCallable)
	void AddOnWepAvailabilityChanged(uint8 Slot, const FOnWepAvailabilityChangedSingle& Delegate);

	UFUNCTION(BlueprintCallable) void SlotP(uint8 Slot);
	UFUNCTION(BlueprintCallable) void SlotR(uint8 Slot);
	UFUNCTION(BlueprintCallable) void FireP();
	UFUNCTION(BlueprintCallable) void FireR();

	struct FBroadcastEquipWeapon;
	struct FBroadcastAvailabilityChanged;

protected:
	void InitializeComponent() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void StartFire();
	void StopFire();
	
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireR();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireP();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireR();

	UFUNCTION(Server, Reliable, WithValidation) void ServerSlotP(uint8 Slot);
	UFUNCTION(Server, Reliable, WithValidation) void ServerSlotR(uint8 Slot);
	UFUNCTION(NetMulticast, Reliable) void MulticastSlotP(uint8 Slot);
	UFUNCTION(NetMulticast, Reliable) void MulticastSlotR(uint8 Slot);

	UFUNCTION()
	void OnRep_Weapons();

	void ClearWeapons();

	FOnEquipWeapon OnEquipWeapon;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<AWeapon>> Weapons;

	// BUG: UE5.0.3에서 배열이 리플리케이트되지 않는 버그가 있음. 이를 위한 임시 방편
	UPROPERTY(ReplicatedUsing=OnRep_Weapons)
	TObjectPtr<AWeapon> Weapon0;
	
	// BUG: UE5.0.3에서 배열이 리플리케이트되지 않는 버그가 있음. 이를 위한 임시 방편
	UPROPERTY(ReplicatedUsing=OnRep_Weapons)
	TObjectPtr<AWeapon> Weapon1;
	
	TArray<FOnWepAvailabilityChanged> OnWepAvailabilityChanged;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 WeaponSlots = 2;

	UPROPERTY(VisibleInstanceOnly, Replicated, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Active;

	UPROPERTY(EditInstanceOnly)
	uint8 bFirePressed : 1;
	
	UPROPERTY(VisibleInstanceOnly)
	uint8 bShouldAutoFire : 1;
	
	UPROPERTY(VisibleInstanceOnly)
	uint8 bFiring : 1;
};

struct UWeaponComponent::FBroadcastEquipWeapon
{
private:
	friend AWeapon;
	
	FBroadcastEquipWeapon(UWeaponComponent* WeaponComponent, AWeapon* Weapon)
	{
		WeaponComponent->OnEquipWeapon.Broadcast(Weapon);
	}
};

struct UWeaponComponent::FBroadcastAvailabilityChanged
{
private:
	friend AWeapon;
	FBroadcastAvailabilityChanged(UWeaponComponent* Comp, const AWeapon* Wep, bool bAvailable);
};
