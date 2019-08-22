// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Player/SaucewichPlayerState.h"
#include "MakeSandwichPlayerState.generated.h"

UCLASS()
class SAUCEWICH_API AMakeSandwichPlayerState final : public ASaucewichPlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PickupIngredient(TSubclassOf<class ASandwichIngredient> Class);
	
	UFUNCTION(BlueprintCallable)
	uint8 GetNumIngredients() const;

	UFUNCTION(BlueprintCallable)
	bool CanPickupIngredient() const;

protected:
	void OnDeath() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPickupIngredient(TSubclassOf<class ASandwichIngredient> Class);

	void DropIngredients();

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TMap<TSubclassOf<class ASandwichIngredient>, uint8> Ingredients;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxIngredients;
};
