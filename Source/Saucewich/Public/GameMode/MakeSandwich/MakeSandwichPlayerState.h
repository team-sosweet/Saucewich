// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Player/SaucewichPlayerState.h"
#include "MakeSandwichPlayerState.generated.h"

class ASandwichIngredient;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerIngredientChanged, class AMakeSandwichPlayerState*, PlayerState);
DECLARE_EVENT_OneParam(AMakeSandwichPlayerState, FOnPlyIngChangedNative, AMakeSandwichPlayerState*)

USTRUCT(BlueprintType)
struct SAUCEWICH_API FIngredients
{
	GENERATED_BODY()

	FIngredients() = default;
	explicit FIngredients(AMakeSandwichPlayerState* const Owner);
	
	auto& Get() const { return Ingredients; }

	template <class Fn>
	decltype(auto) Modify(Fn&& Func)
	{
		Func(Ingredients);
		OnModify();
	}

private:
	void OnModify() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TMap<TSubclassOf<ASandwichIngredient>, uint8> Ingredients;

	UPROPERTY()
	AMakeSandwichPlayerState* Owner;
};

UCLASS()
class SAUCEWICH_API AMakeSandwichPlayerState : public ASaucewichPlayerState
{
	GENERATED_BODY()

public:
	AMakeSandwichPlayerState();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void PickupIngredient(TSubclassOf<ASandwichIngredient> Class);

	void PutIngredientsInFridge();
	void BroadcastIngredientChanged();
	auto& GetIngredients() const { return Ingredients.Get(); }

	UFUNCTION(BlueprintCallable)
	uint8 GetNumIngredients() const;

	UFUNCTION(BlueprintCallable)
	bool CanPickupIngredient() const;

	FOnPlyIngChangedNative OnIngChangedNative;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FIngredients Ingredients;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerIngredientChanged OnIngredientChanged;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxIngredients;
};
