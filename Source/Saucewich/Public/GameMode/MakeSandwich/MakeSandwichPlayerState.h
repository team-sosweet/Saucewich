// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Player/SaucewichPlayerState.h"
#include "MakeSandwichPlayerState.generated.h"

class ASandwichIngredient;

UCLASS()
class SAUCEWICH_API AMakeSandwichPlayerState : public ASaucewichPlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void PickupIngredient(TSubclassOf<ASandwichIngredient> Class);

	void PutIngredientsInFridge();

	auto& GetIngredients() const { return Ingredients; }

	UFUNCTION(BlueprintCallable)
	uint8 GetNumIngredients() const;

	UFUNCTION(BlueprintCallable)
	bool CanPickupIngredient() const;

	ASandwichIngredient* PickingUp;

protected:
	void Reset() override;
	void OnDeath() override;
	void OnCharDestroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPutIngredients();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPickupIngredient();

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPickupIngredient(TSubclassOf<ASandwichIngredient> Class);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetIngredients();
	
	void DropIngredients();

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TMap<TSubclassOf<ASandwichIngredient>, uint8> Ingredients;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxIngredients;
};
