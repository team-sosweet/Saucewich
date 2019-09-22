// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "HttpGameInstance.h"
#include "SaucewichGameMode.generated.h"

class ASaucewichPlayerController;

UCLASS()
class SAUCEWICH_API ASaucewichGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASaucewichGameMode();

	void SetPlayerRespawnTimer(ASaucewichPlayerController* PC) const;
	float GetNextGameWaitTime() const { return NextGameWaitTime; }
	float GetPickupSpawnInterval() const { return PickupSpawnInterval; }

	UFUNCTION(BlueprintCallable)
	void PrintMessage(FName MessageID, float Duration = 3) const;
	auto GetMessage(const FName ID) const { return Messages.Find(ID); }

	auto& GetAvailableMaps() const { return Maps; }

protected:
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void BeginPlay() override;

	APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;
	
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	void GenericPlayerInitialization(AController* C) override;

	bool ShouldSpawnAtStartSpot(AController* Player) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;

	bool ReadyToStartMatch_Implementation() override;
	bool ReadyToEndMatch_Implementation() override;
	void HandleMatchHasStarted() override;
	void HandleMatchHasEnded() override;

private:
	void UpdateMatchState();
	void StartNextGame() const;

	void ExtUpdatePlyCnt() const;

	UFUNCTION()
	void RespondExtUpdatePlyCnt(bool bIsSuccess, int32 Code, FJson Json);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UWorld>> Maps;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FText> Messages;

	FTimerHandle MatchStateTimer;
	FTimerHandle MatchStateUpdateTimer;
	FTimerHandle ExtPlyCntUpdateTimer;

	// 게임이 끝나고 다음 게임을 시작하기까지 기다리는 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true, UIMin=0))
	float NextGameWaitTime = 10;
	
	// 게임 시작 대기하다가 게임을 시작하려고 하기까지의 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true, UIMin=0))
	float MatchStartingTime = 2;
	
	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
	float PickupSpawnInterval = 20;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
	float MatchStateUpdateInterval = 1;

	UPROPERTY(EditDefaultsOnly)
	uint8 MinPlayerToStart = 2;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxPlayers = 8;

	uint8 bAboutToStartMatch : 1;
};
