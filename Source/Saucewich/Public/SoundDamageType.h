// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/DamageType.h"
#include "SoundDamageType.generated.h"

UCLASS(Const)
class SAUCEWICH_API USoundDamageType final : public UDamageType
{
	GENERATED_BODY()

public:
	class USoundBase* GetSound() const;

private:
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<USoundBase>> Sounds;
};
