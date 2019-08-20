// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Player/SaucewichPlayerState.h"
#include "MakeSandwichPlayerState.generated.h"

/**
 * 샌드위치 만들기 게임 모드 전용 플레이어 스테이트 입니다.
 */
UCLASS()
class SAUCEWICH_API AMakeSandwichPlayerState final : public ASaucewichPlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PickupIngredient(TSubclassOf<class APickup> Class);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPickupIngredient(TSubclassOf<class APickup> Class);

	UFUNCTION(BlueprintCallable)
	uint8 GetNumIngredients() const;

	UFUNCTION(BlueprintCallable)
	bool CanPickupIngredient() const;

protected:
	void OnDeath() override;

private:
	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TMap<TSubclassOf<APickup>, uint8> Ingredients;

	UPROPERTY(EditDefaultsOnly)
	uint8 MaxIngredients;
};
