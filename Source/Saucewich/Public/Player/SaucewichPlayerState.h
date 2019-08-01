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
	uint8 GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable)
	void SetTeam(uint8 NewTeam);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetWeapon(uint8 Slot, UClass* Weapon);
	void GiveWeapons();

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChangedDelegate;

protected:
	UFUNCTION() void OnRep_Weapons();
	UFUNCTION() void OnTeamChanged() const;

	void PostInitializeComponents() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 플레이어가 무기를 선택하지 않았을 때 기본적으로 지급될 무기입니다.
	// 배열 인덱스는 무기 슬롯을 의미합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

	UPROPERTY(ReplicatedUsing=OnRep_Weapons, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSubclassOf<AWeapon>> Weapons;

	UPROPERTY(ReplicatedUsing=OnTeamChanged, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
