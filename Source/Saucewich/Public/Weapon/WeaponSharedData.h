// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "WeaponSharedData.generated.h"

UCLASS()
class SAUCEWICH_API UWeaponSharedData : public UDataAsset
{
	GENERATED_BODY()

public:
	class UMaterialInterface* GetTranslMat(const UMaterialInterface* OrigMat) const;
	
	UPROPERTY(EditAnywhere)
	FName ColMatName;

private:
	UPROPERTY(EditAnywhere)
	const class UTranslMatData* SharedTranslMat;
};
