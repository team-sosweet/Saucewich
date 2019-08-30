// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "SandwichIngredient.generated.h"

UCLASS(Abstract)
class SAUCEWICH_API ASandwichIngredient final : public APickup
{
	GENERATED_BODY()

protected:
	void BePickedUp(ATpsCharacter* By) override;
	void StartPickUp(ATpsCharacter* By) override;
	void CancelPickUp(ATpsCharacter* By) override;
	bool CanPickedUp(const ATpsCharacter* By) const override;
	bool CanEverPickedUp(const ATpsCharacter* By) const override;
};
