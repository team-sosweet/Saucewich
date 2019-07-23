// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void SetupPlayerInputComponent(class UInputComponent* Input);

	/*
	 * [Server] Gives a weapon. Replaces if already in the same slot.
	 * @param WeaponClass: Class of weapon to give
	 * @return: The weapon given
	 */
	UFUNCTION(BlueprintCallable)
	virtual AWeapon* Give(TSubclassOf<AWeapon> WeaponClass);

	/*
	 * [Shared] Returns active weapon.
	 * @return: The weapon currently equipped
	 */
	UFUNCTION(BlueprintCallable)
	AWeapon* GetActiveWeapon() const { return Active < Weapons.Num() ? Weapons[Active] : nullptr; }

	virtual bool TrySelectWeapon(uint8 Slot);
	
	class ATpsCharacter* const Owner = nullptr;

protected:
	void InitializeComponent() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnRep_Weapons();

private:
	void FireP(); void FireR();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireP();
	UFUNCTION(Server, Reliable, WithValidation) void ServerFireR();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireP();
	UFUNCTION(NetMulticast, Reliable) void MulticastFireR();

	void SlotP(uint8 Slot); void SlotR(uint8 Slot);
	UFUNCTION(Server, Reliable, WithValidation) void ServerSlotP(uint8 Slot);
	UFUNCTION(Server, Reliable, WithValidation) void ServerSlotR(uint8 Slot);
	UFUNCTION(NetMulticast, Reliable) void MulticastSlotP(uint8 Slot);
	UFUNCTION(NetMulticast, Reliable) void MulticastSlotR(uint8 Slot);
	
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing=OnRep_Weapons, Transient)
	TArray<AWeapon*> Weapons;

	UPROPERTY(EditDefaultsOnly, meta = (UIMax = 9, ClampMax = 9))
	uint8 WeaponSlots;

	UPROPERTY(VisibleInstanceOnly, Replicated, Transient)
	uint8 Active;
};
