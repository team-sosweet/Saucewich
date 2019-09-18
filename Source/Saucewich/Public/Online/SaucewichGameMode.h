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
	float GetPickupSpawnInterval() const { return PickupSpawnInterval; }

	UFUNCTION(BlueprintCallable)
	void PrintMessage(FName MessageID, float Duration = 3) const;
	auto GetMessage(const FName ID) const { return Messages.Find(ID); }

protected:
	APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	bool FindInactivePlayer(APlayerController* PC) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;
	void HandleMatchHasEnded() override;

private:
	void StartNextGame() const;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UWorld>> Maps;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FText> Messages;

	FTimerHandle NextGameTimer;
	
	// 게임이 끝나고 다음 게임을 시작하기까지 기다리는 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true, UIMin=0))
	float NextGameWaitTime = 10;
	
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
	float PickupSpawnInterval = 20;

	uint8 LastTeam;
};
