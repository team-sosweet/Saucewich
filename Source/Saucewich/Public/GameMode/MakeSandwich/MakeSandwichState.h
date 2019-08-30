// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Online/SaucewichGameState.h"
#include "MakeSandwichState.generated.h"

class ASandwichIngredient;
class AMakeSandwichPlayerState;

UCLASS()
class SAUCEWICH_API AMakeSandwichState : public ASaucewichGameState
{
	GENERATED_BODY()

public:
	void StoreIngredients(AMakeSandwichPlayerState* Player);

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetIngredients(UClass* Ingredient, uint8 Num, uint8 Team);

	auto& GetTeamIngredients(const uint8 Team)
	{
		if (IngredientsByTeam.Num() <= Team) IngredientsByTeam.SetNum(Team + 1);
		return IngredientsByTeam[Team];
	}
	
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Team Ingredients"))
	const TMap<TSubclassOf<ASandwichIngredient>, uint8>& BP_GetTeamIngredients(const uint8 Team)
	{
		return GetTeamIngredients(Team);
	}

	TArray<TMap<TSubclassOf<ASandwichIngredient>, uint8>> IngredientsByTeam;
};
