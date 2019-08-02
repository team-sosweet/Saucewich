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
	void SetPlayerRespawnTimer(ASaucewichPlayerController* PC) const;

protected:
	void PostInitializeComponents() override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

	void GiveWeapons(class ATpsCharacter* Character);

private:
	class ASaucewichGameState* State;
};
