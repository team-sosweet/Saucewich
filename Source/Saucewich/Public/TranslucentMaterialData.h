// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TranslucentMaterialData.generated.h"

/**
 * 어떤 머티리얼에 대한 반투명 머티리얼을 저장합니다.
 */
UCLASS()
class SAUCEWICH_API UTranslucentMaterialData : public UDataAsset
{
	GENERATED_BODY()

public:
	auto Get(const TSoftObjectPtr<class UMaterialInterface> Material) const
	{
		return Data.Find(Material);
	}

private:
	UPROPERTY(EditAnywhere)
	TMap<TSoftObjectPtr<UMaterialInterface>, TSoftObjectPtr<UMaterialInterface>> Data;
};
