// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameMode/SaucewichGameState.h"
#include "MakeSandwichState.generated.h"

class ASandwichIngredient;
class AMakeSandwichPlayerState;

UCLASS()
class SAUCEWICH_API AMakeSandwichState : public ASaucewichGameState
{
	GENERATED_BODY()

public:
	void StoreIngredients(AMakeSandwichPlayerState* Player);
	auto& GetRequiredIngredients() const { return SandwichIngredients; }

	auto& GetTeamIngredients(const uint8 Team)
	{
		if (IngredientsByTeam.Num() <= Team) IngredientsByTeam.AddDefaulted(Team - IngredientsByTeam.Num() + 1);
		return IngredientsByTeam[Team];
	}
	
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Get Team Ingredients"))
	const TMap<UClass*, uint8>& BP_GetTeamIngredients(const uint8 Team)
	{
		return reinterpret_cast<const TMap<UClass*, uint8>&>(GetTeamIngredients(Team));
	}

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetIngredients(uint8 Team, const TArray<TSubclassOf<ASandwichIngredient>>& Ingredients, const TArray<uint8>& Num);

	// 샌드위치 하나를 만드는데 필요한 재료
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSet<TSubclassOf<ASandwichIngredient>> SandwichIngredients;

	TArray<TMap<TSubclassOf<ASandwichIngredient>, uint8>> IngredientsByTeam;
};
