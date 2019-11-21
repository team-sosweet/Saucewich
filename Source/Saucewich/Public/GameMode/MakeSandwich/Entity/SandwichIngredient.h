// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "SandwichIngredient.generated.h"

UCLASS(Abstract)
class SAUCEWICH_API ASandwichIngredient : public APickup
{
	GENERATED_BODY()

protected:
	void OnPickedUp_Implementation(ATpsCharacter* By) override;
	void StartPickUp_Implementation(ATpsCharacter* By) override;
	void CancelPickUp_Implementation(ATpsCharacter* By) override;
	bool CanPickedUp_Implementation(const ATpsCharacter* By) const override;
	bool CanEverPickedUp_Implementation(const ATpsCharacter* By) const override;
};
