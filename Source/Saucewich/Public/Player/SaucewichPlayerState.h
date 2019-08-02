// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SaucewichPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameStateReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, uint8, NewTeam);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	uint8 GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable)
	void SetTeam(uint8 NewTeam);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetWeapon(uint8 Slot, TSubclassOf<class AWeapon> Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWeapon(uint8 Slot, TSubclassOf<AWeapon> Weapon);

	void GiveWeapons();

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChangedDelegate;

protected:
	UFUNCTION()
	void OnTeamChanged(uint8 OldTeam);

	void PostInitializeComponents() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Init();

	template <class Fn>
	void SafeGameState(Fn&& Func)
	{
		if (GameState) Func();
		else OnGameStateReady.AddLambda(std::move(Func));
	}

	FOnGameStateReady OnGameStateReady;

	// 플레이어가 무기를 선택하지 않았을 때 기본적으로 지급될 무기입니다.
	// 배열 인덱스는 무기 슬롯을 의미합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<AWeapon>> DefaultWeapons;

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<AWeapon>> Weapons;

	class ASaucewichGameState* GameState;

	UPROPERTY(ReplicatedUsing=OnTeamChanged, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
