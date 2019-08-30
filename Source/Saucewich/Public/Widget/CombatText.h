// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatText.generated.h"

/**
 * 
 */
UCLASS()
class SAUCEWICH_API UCombatText final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void ViewCombatText(float Damage, class ATpsCharacter* DamagedActor);

private:
	void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
	FVector2D GetRandomPos();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector2D MinExtendSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector2D MaxExtendSize;

	UPROPERTY(Transient)
	class USizeBox* SizeBox;

	UPROPERTY(Transient)
	class UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* Fade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FLinearColor Color;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MinDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxDamage;
};
