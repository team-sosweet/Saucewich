// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "WeaponSharedData.generated.h"

UCLASS()
class SAUCEWICH_API UWeaponSharedData final : public UDataAsset
{
	GENERATED_BODY()

public:
	class UMaterialInterface* GetTranslMat(const UMaterialInterface* OrigMat) const;
	
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FName ColMatName;

private:
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	const class UTranslMatData* SharedTranslMat;
};
