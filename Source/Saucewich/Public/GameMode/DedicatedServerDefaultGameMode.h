// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "HttpGameInstance.h"
#include "DedicatedServerDefaultGameMode.generated.h"

UCLASS()
class SAUCEWICH_API ADedicatedServerDefaultGameMode final : public AGameModeBase
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

private:
	UFUNCTION()
	void OnServerRegistered(bool bIsSuccess, int32 Code, FJson Json);

	void StartServer() const;
};
