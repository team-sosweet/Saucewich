// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "CharacterData.generated.h"

UCLASS()
class SAUCEWICH_API UCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	UMaterialInterface* GetTranslMat(uint8 Idx, const UMaterialInterface* Mat) const;

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<USoundBase>> DeathSounds;
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UParticleSystem> DeathFX;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UForceFeedbackEffect> DeathFBB;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UCameraShakeBase> DeathShake;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UCameraShakeBase> HitShake;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FName ColMatName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP = 100;

	// 스폰 무적 시간
	UPROPERTY(EditAnywhere,  BlueprintReadOnly)
	float RespawnInvincibleTime;

	UPROPERTY(EditAnywhere)
	float DeathSauceMarkScale = 1.f;

private:
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	TMap<uint8, UMaterialInterface*> TranslMatOverride;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	const class UTranslMatData* SharedTranslMat;
};
