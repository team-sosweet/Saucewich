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
	FColor Color;
};

UCLASS()
class SAUCEWICH_API ASaucewichGameState : public AGameState
{
	GENERATED_BODY()

public:
	const FTeam& GetTeamData(const uint8 Team) const { return Teams[Team]; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<FTeam> Teams;
};
