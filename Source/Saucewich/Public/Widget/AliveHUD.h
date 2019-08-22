// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AliveHUD.generated.h"

UCLASS()
class SAUCEWICH_API UAliveHUD final : public UUserWidget
{
	GENERATED_BODY()
	
	void NativeOnInitialized() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	float GetHealthBarThickness() const;

private:
	UFUNCTION()
	void SetTeamColor(uint8 NewTeam);

	void BindOnTeamChanged();
	
	void AddProgressBarMaterial(class UProgressBar* ProgressBar, UTexture* Icon, UTexture* Mask);

	UPROPERTY(Transient)
	UProgressBar* HealthProgressBar;

	UPROPERTY(Transient)
	UProgressBar* ClipProgressBar;

	UPROPERTY(Transient)
	UProgressBar* SubWeaponProgressBar;

	UPROPERTY(Transient)
	class UButton* SubWeaponButton;

	UPROPERTY(Transient)
	class UBorder* AttackButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMaterialInterface* IconMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BindData, meta = (AllowPrivateAccess = true))
	FLinearColor MyTeamColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BindData, meta = (AllowPrivateAccess = true))
	FLinearColor EnemyTeamColor;

	UPROPERTY(Transient)
	TArray<class UMaterialInstanceDynamic*> Materials;

	UPROPERTY()
	class ASaucewichGameState* GameState;
};
