// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "TranslMatData.generated.h"

/**
 * 어떤 머티리얼에 대한 반투명 머티리얼을 저장합니다.
 */
UCLASS()
class SAUCEWICH_API UTranslMatData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<TSoftObjectPtr<UMaterialInterface>, TSoftObjectPtr<UMaterialInterface>> Map;
};
