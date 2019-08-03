// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class SAUCEWICH_API UBaseHUD : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeOnInitialized() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	float GetHealthBarThickness() const;

private:
	UFUNCTION()
	void SetTeamColor(uint8 NewTeam);

	void BindOnTeamChanged();
	void AddProgressBarMaterial(class UProgressBar* ProgressBar, UTexture* Icon, UTexture* Mask);

private:
	UPROPERTY()
	UProgressBar* HealthProgressBar;

	UPROPERTY()
	UProgressBar* ClipProgressBar;

	UPROPERTY()
	UProgressBar* SubWeaponProgressBar;

	UPROPERTY()
	class UButton* SubWeaponButton;

	UPROPERTY()
	class UButton* AttackButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMaterialInterface* IconMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BindData, meta = (AllowPrivateAccess = true))
	FLinearColor MyTeamColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BindData, meta = (AllowPrivateAccess = true))
	FLinearColor EnemyTeamColor;

	TArray<class UMaterialInstanceDynamic*> Materials;

	class ASaucewichGameState* GameState;
};
