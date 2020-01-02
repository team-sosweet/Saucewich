// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "SaucewichGameState.generated.h"

class AWeapon;
class ASauceMarker;
class ATpsCharacter;
class ASaucewichPlayerState;

struct FGameData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerChangedTeam, ASaucewichPlayerState*, Player, uint8, OldTeam, uint8, NewTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDeath, ASaucewichPlayerState*, Victim, ASaucewichPlayerState*, Attacker, AActor*, Inflictor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchEnd, uint8, WonTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChanged, FName, NewMatchState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeavingMap);
DECLARE_EVENT(ASaucewichGameState, FOnCleanupGame)

UCLASS()
class SAUCEWICH_API ASaucewichGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASaucewichGameState();

	UFUNCTION(BlueprintCallable)
	uint8 GetMinPlayerTeam() const;
	
	UFUNCTION(BlueprintCallable)
	TArray<ASaucewichPlayerState*> GetPlayersByTeam(uint8 Team) const;

	UFUNCTION(BlueprintCallable)
	TArray<ATpsCharacter*> GetCharactersByTeam(uint8 Team) const;

	UFUNCTION(BlueprintCallable)
	uint8 GetNumPlayers(uint8 Team) const;

	uint8 GetWinningTeam() const;
	uint8 GetEmptyTeam() const;

	UFUNCTION(BlueprintCallable)
	int32 GetTeamScore(const uint8 Team) const { return TeamScore.Num() <= Team ? 0 : TeamScore[Team]; }
	void SetTeamScore(uint8 Team, int32 NewScore);

	virtual bool CanAddPersonalScore() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerDeath(ASaucewichPlayerState* Victim, ASaucewichPlayerState* Attacker, AActor* Inflictor);

	// 현재 라운드의 남은 시간을 구합니다.
	UFUNCTION(BlueprintCallable)
	float GetRemainingRoundSeconds() const;

	UFUNCTION(BlueprintCallable)
	void AddDilatableActor(AActor* Actor) { DilatableActors.Add(Actor); }
	void AddDilatablePSC(class UParticleSystemComponent* PSC) { DilatablePSCs.Add(PSC); }


	UPROPERTY(BlueprintAssignable)
	FOnPlayerChangedTeam OnPlayerChangedTeam;

	/**
	 * 캐릭터가 죽을 때 호출됩니다.
	 * @param Victim		죽은 캐릭터
	 * @param Attacker		Victim을 죽인 컨트롤러
	 * @param Inflictor		Victim을 죽이는 데 사용된 액터. 대부분의 경우 무기입니다. 해당 액터의 bReplicates가 false면 Client에서는 null입니다.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;

	UPROPERTY(BlueprintAssignable)
	FOnMatchEnd OnMatchEnd;

	UPROPERTY(BlueprintAssignable)
	FOnLeavingMap OnLeavingMap;

	FOnCleanupGame OnCleanup;

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	void OnRep_MatchState() override;
	void HandleMatchHasStarted() override;
	void HandleMatchHasEnded() override;
	void HandleLeavingMap() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void HandleMatchEnding();

private:
	const FGameData& GetGmData() const;
	
	UFUNCTION()
	void OnRep_WonTeam() const;

	UPROPERTY(Transient)
	TSet<UParticleSystemComponent*> DilatablePSCs;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly)
	TArray<int32> TeamScore;

	UPROPERTY(Transient)
	TArray<AActor*> DilatableActors;

	UPROPERTY(BlueprintAssignable)
	FOnMatchStateChanged OnMatchStateChanged;

	FTimerHandle RoundTimer;
	FTextFormat TeamScoreAddMsgFmt;
	
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly)
	float RoundStartTime = -1;

	float Dilation;

	UPROPERTY(ReplicatedUsing=OnRep_WonTeam, Transient)
	uint8 WonTeam = -2;
};
