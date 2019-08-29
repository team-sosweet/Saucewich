// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "SandwichIngredient.generated.h"

UCLASS(Abstract)
class SAUCEWICH_API ASandwichIngredient final : public APickup
{
	GENERATED_BODY()

protected:
	void BePickedUp(AActor* By) override;
	void StartPickUp(AActor* By) override;
	void CancelPickUp(AActor* By) override;
	bool CanPickedUp(const AActor* By) const override;
	bool CanEverPickedUp(const AActor* By) const override;
};
