// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttackButton.generated.h"

UCLASS()
class SAUCEWICH_API UAttackButton final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Percent"))
	float GetPercent();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Weapon Changed"))
	void OnWeaponChanged(class AWeapon* Weapon);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Created Material"))
	void OnCreatedMaterial();
	
private:
	UFUNCTION()
	void OnChangedColor(const FLinearColor& MyTeamColor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMaterialInterface* CoolMaterialParent;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMaterialInstanceDynamic* CoolMaterial;

	UPROPERTY(Transient)
	class UProgressBar* CoolProgressBar;
};
