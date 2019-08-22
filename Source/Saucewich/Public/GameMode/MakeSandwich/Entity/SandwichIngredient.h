// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "SandwichIngredient.generated.h"

UCLASS(Abstract)
class SAUCEWICH_API ASandwichIngredient final : public APickup
{
	GENERATED_BODY()

protected:
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	void OnReleased() override;

private:
	void BePickedUp(class AMakeSandwichPlayerState* Player);
	void ClearTimer(class AMakeSandwichPlayerState* Player);
	
	// 재료를 획득하는데 걸리는 시간
	UPROPERTY(EditDefaultsOnly)
	float PickupTime = 1;
	
	TMap<class AMakeSandwichPlayerState*, FTimerHandle> PickupTimers;
};
