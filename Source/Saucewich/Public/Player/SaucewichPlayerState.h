// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SaucewichPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, uint8, NewTeam);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void SetTeam(uint8 NewTeam);
	uint8 GetTeam() const { return Team; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetWeapon(uint8 Slot, TSubclassOf<class AWeapon> Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWeapon(uint8 Slot, TSubclassOf<AWeapon> Weapon);

	void GiveWeapons();

	virtual void OnDeath() {}

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChangedDelegate;

protected:
	UFUNCTION()
	void OnTeamChanged(uint8 OldTeam);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:	
	// 현재 이 플레이어가 장착한 무기입니다. 리스폰시 지급됩니다.
	// 배열 인덱스는 무기 슬롯을 의미합니다.
	// 기본값을 설정해두면 저장된 로드아웃이 없을 경우 기본값을 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<AWeapon>> Weapons;

	// 플레이어의 팀을 나타냅니다. 팀 번호는 1부터 시작합니다.
	// 팀 관련 함수들은 SaucewichGameState를 확인하세요.
	UPROPERTY(ReplicatedUsing=OnTeamChanged, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
