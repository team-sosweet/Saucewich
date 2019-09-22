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

	UFUNCTION()
	void OnProcessRegistered(bool bIsSuccess, int32 Code, FJson Json);

	void TryStartServer() const;
	void StartServer() const;

	uint8 bRegSv : 1;
	uint8 bRegProc : 1;
};
