// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "SaucewichGameState.generated.h"

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
	TArray<AActor*> GetCharacters(uint8 Team) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FTeam> Teams;
};
