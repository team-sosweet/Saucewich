// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "Saucewich.h"
#include "SaucewichGameMode.generated.h"

class AWeapon;
class APlayerStart;
class USaucewichInstance;
class ASaucewichPlayerController;

namespace MatchState
{
	extern const FName Ending;
}

USTRUCT(BlueprintType)
struct SAUCEWICH_API FTeam
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Color;
};

USTRUCT(BlueprintType)
struct FGameData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UWorld>> Maps;

	UPROPERTY(EditDefaultsOnly)
	FString StreamLevelSuffix;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTeam> Teams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RoundMinutes = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=0))
	float NextGameWaitTime = 10;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=0))
	float MatchStartingTime = 2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=0))
	float PickupSpawnInterval = 20;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
	float MatchStateUpdateInterval = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MinPlayerToStart = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxPlayers = 6;
};

UCLASS()
class SAUCEWICH_API ASaucewichGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Game Data", WorldContext=WorldContextObj))
	static const FGameData& GetData(const UObject* WorldContextObj);
	
	ASaucewichGameMode();
	
	auto&& GetData() const { return Data; }
	void StartNextGame() const;

	UFUNCTION(BlueprintCallable)
	void PrintMessage(const FText& Msg, EMsgType Type, float Duration = 3) const;

	TSoftClassPtr<ASaucewichGameMode> ChooseNextGameMode() const;
	TSoftObjectPtr<UWorld> ChooseNextMap() const;

	void SetPlayerRespawnTimer(ASaucewichPlayerController* PC) const;
	void OnPlayerChangedName(class ASaucewichPlayerState* Player, FString&& OldName) const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void HandleMatchEnding();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void BeginPlay() override;

	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;	
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;
	
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	void GenericPlayerInitialization(AController* C) override;
	bool FindInactivePlayer(APlayerController* PC) override { return false; }
	void AddInactivePlayer(APlayerState* PlayerState, APlayerController* PC) override {}

	bool ShouldSpawnAtStartSpot(AController* Player) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;

	void OnMatchStateSet() override;
	bool ReadyToStartMatch_Implementation() override;
	bool ReadyToEndMatch_Implementation() override;
	void HandleMatchHasStarted() override;
	void HandleMatchHasEnded() override;
	void EndMatch() override;

private:
	void UpdateMatchState();
	USaucewichInstance* GetSaucewichInstance() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FGameData Data;

	TArray<TArray<APlayerStart*>> TeamStarts;
	
	FTimerHandle MatchStateTimer;
	FTimerHandle MatchStateUpdateTimer;
	FTimerHandle ExtPlyCntUpdateTimer;

	uint8 bAboutToStartMatch : 1;
};
