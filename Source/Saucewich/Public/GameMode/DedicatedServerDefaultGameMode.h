// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "DedicatedServerDefaultGameMode.generated.h"

UCLASS()
class SAUCEWICH_API ADedicatedServerDefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADedicatedServerDefaultGameMode();

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

private:
	void StartServer() const;

	uint8 bStart : 1;
};
