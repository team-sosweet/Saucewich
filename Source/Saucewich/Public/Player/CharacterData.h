// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "CharacterData.generated.h"

UCLASS()
class SAUCEWICH_API UCharacterData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 기본 최대 체력입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultMaxHP = 100.f;

	// 스폰 무적 시간
	UPROPERTY(EditAnywhere)
	float RespawnInvincibleTime;
};
