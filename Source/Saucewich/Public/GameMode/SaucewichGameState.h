// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "SaucewichGameState.generated.h"

class AWeapon;
class ASauceMarker;
class ATpsCharacter;
class ASaucewichPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerChangedTeam, ASaucewichPlayerState*, Player, uint8, OldTeam, uint8, NewTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDeath, ASaucewichPlayerState*, Victim, ASaucewichPlayerState*, Attacker, AActor*, Inflictor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchEnd, uint8, WonTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeavingMap);
DECLARE_EVENT(ASaucewichGameState, FOnCleanupGame)

USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Color;
};

USTRUCT(BlueprintType)
struct SAUCEWICH_API FScoreData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Score;
};

UCLASS()
class SAUCEWICH_API ASaucewichGameState : public AGameState
{
	GENERATED_BODY()

public:
	class GetSauceMarker;

	UFUNCTION(BlueprintCallable)
	const FTeam& GetTeamData(const uint8 Team) const { return Teams[Team]; }

	UFUNCTION(BlueprintCallable)
	uint8 GetNumTeam() const { return Teams.Num() - 1; }

	UFUNCTION(BlueprintCallable)
	bool IsValidTeam(const uint8 Team) const { return Team > 0 && Team < Teams.Num(); }

	// 플레이어 수가 가장 적은 팀을 반환합니다. 여러 개일 경우 무작위로 반환됩니다.
	UFUNCTION(BlueprintCallable)
	uint8 GetMinPlayerTeam() const;

	
	UFUNCTION(BlueprintCallable)
	TArray<ASaucewichPlayerState*> GetPlayersByTeam(uint8 Team) const;

	UFUNCTION(BlueprintCallable)
	TArray<ATpsCharacter*> GetCharactersByTeam(uint8 Team) const;

	UFUNCTION(BlueprintCallable)
	uint8 GetNumPlayers(uint8 Team) const;


	// 이기고 있는 팀을 반환합니다.
	// 비기고 있는 경우 0을 반환합니다.
	uint8 GetWinningTeam() const;
	uint8 GetEmptyTeam() const;

	UFUNCTION(BlueprintCallable)
	int32 GetTeamScore(const uint8 Team) const { return TeamScore.Num() <= Team ? 0 : TeamScore[Team]; }
	void SetTeamScore(uint8 Team, int32 NewScore);
	
	UFUNCTION(BlueprintCallable)
	const FScoreData& GetScoreData(FName ForWhat) const;

	virtual bool CanAddPersonalScore() const;

	// 무기 목록에서 특정 슬롯의 무기들만 반환합니다
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AWeapon>> GetAvailableWeapons(uint8 Slot) const;
	auto& GetAvailableWeapons() const { return AvailableWeapons; }
	

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayerDeath(ASaucewichPlayerState* Victim, ASaucewichPlayerState* Attacker, AActor* Inflictor);

	virtual bool ShouldPlayerTakeDamage(const ATpsCharacter* Victim, float DamageAmount, const FDamageEvent& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser) const;


	// 현재 라운드의 남은 시간을 구합니다.
	UFUNCTION(BlueprintCallable)
	float GetRemainingRoundSeconds() const;


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
	void HandleMatchHasStarted() override;
	void HandleMatchHasEnded() override;
	void HandleLeavingMap() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	static constexpr uint8 InvalidTeam = -1;
	
	UFUNCTION()
	void OnRep_WonTeam();

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FScoreData> ScoreData;

	// 팀 정보를 저장하는 배열입니다. 게임 플레이 도중 바뀌지 않습니다.
	// 0번째는 unassigned/connecting 팀으로, 사용되지 않는 팀이어야 합니다.
	// 팀 개수는 사용되지 않는 0번 팀 포함 최소 2개여야 합니다.
	// 실제 팀 index는 1부터 시작합니다.
	UPROPERTY(EditDefaultsOnly)
	TArray<FTeam> Teams;

	// 게임에서 사용할 무기 목록입니다.
	// 플레이어는 무기 선택창에서 이 무기들중 하나를 선택하여 사용할 수 있습니다.
	// 특정 슬롯의 무기만을 구하고 싶으면 GetAvailableWeapons 함수를 사용하세요.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSoftClassPtr<AWeapon>> AvailableWeapons;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly)
	TArray<int32> TeamScore;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UMaterialInterface>> SauceMarkMaterials;
	TArray<TArray<const ASauceMarker*>> SauceMarkers;
	

	FTimerHandle RoundTimer;
	
	// 한 판에 걸리는 시간
	UPROPERTY(EditDefaultsOnly)
	float RoundMinutes = 3;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly)
	float RoundStartTime = -1;

	UPROPERTY(ReplicatedUsing=OnRep_WonTeam, Transient)
	uint8 WonTeam = InvalidTeam;
};

class ASaucewichGameState::GetSauceMarker
{
	friend ASauceMarker;
	GetSauceMarker(uint8 Team, const UWorld* World);
	const ASauceMarker* operator->() const { return Marker; }
	const ASauceMarker* Marker;
};
