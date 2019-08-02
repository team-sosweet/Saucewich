// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "SaucewichGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerChangedWeapon, class ASaucewichPlayerState*, Player, uint8, Slot, TSubclassOf<class AWeapon>, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerChangedTeam, ASaucewichPlayerState*, Player, uint8, OldTeam, uint8, NewTeam);

USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Color;
};

UCLASS()
class SAUCEWICH_API ASaucewichGameState : public AGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	const FTeam& GetTeamData(const uint8 Team) const { return Teams[Team]; }

	UFUNCTION(BlueprintCallable)
	TArray<class ASaucewichPlayerState*> GetPlayers(uint8 Team) const;

	UFUNCTION(BlueprintCallable)
	TArray<class ATpsCharacter*> GetCharacters(uint8 Team) const;

	bool IsValidTeam(const uint8 Team) const { return Team > 0 && Team < Teams.Num(); }
	uint8 GetPlayerNum(uint8 Team) const;

	// 플레이어 수가 가장 적은 팀을 반환합니다. 여러 개일 경우 무작위로 반환됩니다.
	uint8 GetMinPlayerTeam() const;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerChangedWeapon OnPlayerChangedWeapon;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerChangedTeam OnPlayerChangedTeam;

private:
	/*
	 * 팀 정보를 저장하는 배열입니다. 게임 플레이 도중 바뀌지 않습니다.
	 * 0번 요소는 unassigned/connecting 팀으로, 사용되지 않는 팀이어야 합니다.
	 * 팀 개수는 사용되지 않는 0번 팀 포함 최소 2개여야 합니다.
	 * 실제 팀 index는 1부터 시작합니다.
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray<FTeam> Teams;
};
