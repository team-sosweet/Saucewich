// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SaucewichPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreAdded, FName, ScoreID, int32, ActualScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, uint8, NewTeam);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	void SetTeam(uint8 NewTeam);
	uint8 GetTeam() const { return Team; }

	void SetWeapon(uint8 Slot, TSubclassOf<class AWeapon> Weapon);

	UFUNCTION(BlueprintCallable)
	void SaveWeaponLoadout() const;

	void GiveWeapons();

	virtual void OnKill();
	virtual void OnDeath();
	virtual void OnCharDestroyed() {}

	/**
	 * 점수를 추가(혹은 차감)합니다. Authority 전용입니다.
	 * @param ScoreID		점수 ID
	 * @param ActualScore	0이 아닌 값이면 미리 지정된 점수 말고 이 값만큼 추가
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddScore(FName ScoreID, int32 ActualScore = 0);

	uint8 GetKill() const { return Kill; }
	uint8 GetDeath() const { return Death; }
	uint8 GetObjective() const { return Objective; }
	void SetObjective(uint8 NewObjective);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void RequestSetPlayerName(const FString& NewPlayerName);

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChangedDelegate;

	/**
	 * 점수가 추가될때 호출됩니다.
	 * @warning 클라이언트에서는 이 델리게이트가 호출된 시점에서 점수 추가가 실제로 반영되었다는 보장이 없습니다.
	 * @param ScoreName 이 점수의 이름입니다. GameState에서 점수 정보를 얻을 때 사용하는 식별자입니다.
	 * @param ActualScore 실제로 주어진 점수입니다.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnScoreAdded OnScoreAdded;

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnTeamChanged(uint8 OldTeam);

private:
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerSetWeapon(uint8 Slot, TSubclassOf<AWeapon> Weapon);
	void SetWeapon_Internal(uint8 Slot, TSubclassOf<AWeapon> Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddScore(FName ScoreID, int32 ActualScore);

	UFUNCTION()
	void LoadWeaponLoadout(class ATpsCharacter* Char);

	void NotifySpawnToController();

	// 현재 이 플레이어가 장착한 무기입니다. 리스폰시 지급됩니다.
	// 배열 인덱스는 무기 슬롯을 의미합니다.
	// 기본값을 설정해두면 저장된 로드아웃이 없을 경우 기본값을 사용합니다.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<AWeapon>> WeaponLoadout;

	// 플레이어의 팀을 나타냅니다. 팀 번호는 1부터 시작합니다.
	// 팀 관련 함수들은 SaucewichGameState를 확인하세요.
	UPROPERTY(ReplicatedUsing=OnTeamChanged, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
	
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Objective;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Kill;
	
	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Death;
};
