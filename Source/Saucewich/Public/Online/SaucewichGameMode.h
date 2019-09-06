// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "SaucewichPlayerController.h"
#include "SaucewichGameMode.generated.h"

UCLASS()
class SAUCEWICH_API ASaucewichGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASaucewichGameMode();

	virtual void UpdateMatchState();
	void SetPlayerRespawnTimer(ASaucewichPlayerController* PC) const;

protected:
	APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	bool FindInactivePlayer(APlayerController* PC) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;

private:
	uint8 LastTeam;
};
