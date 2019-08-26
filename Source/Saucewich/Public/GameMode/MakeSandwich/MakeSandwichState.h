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

	TMap<TSubclassOf<ASandwichIngredient>, uint8> TeamIngredients[3];
};
