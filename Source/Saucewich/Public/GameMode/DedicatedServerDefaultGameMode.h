// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "DedicatedServerDefaultGameMode.generated.h"

UCLASS()
class SAUCEWICH_API ADedicatedServerDefaultGameMode final : public AGameModeBase
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
};
