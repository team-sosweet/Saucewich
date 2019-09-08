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
	float GetNextGameWaitTime() const { return NextGameWaitTime; }

protected:
	APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	bool FindInactivePlayer(APlayerController* PC) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;

private:
	// 게임이 끝나고 다음 게임을 시작하기까지 기다리는 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float NextGameWaitTime = 10;
	
	uint8 LastTeam;
};
