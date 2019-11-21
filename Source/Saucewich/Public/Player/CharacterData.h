// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "CharacterData.generated.h"

UCLASS()
class SAUCEWICH_API UCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	class UMaterialInterface* GetTranslMat(uint8 Idx, const UMaterialInterface* Mat) const;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FName ColMatName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP = 100;

	// 스폰 무적 시간
	UPROPERTY(EditAnywhere,  BlueprintReadOnly)
	float RespawnInvincibleTime;

private:
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	TMap<uint8, UMaterialInterface*> TranslMatOverride;

	UPROPERTY(EditAnywhere, AdvancedDisplay)
	const class UTranslMatData* SharedTranslMat;
};
