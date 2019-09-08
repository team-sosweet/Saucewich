// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Perk.h"

#include "Player/TpsCharacter.h"

void APerk::BePickedUp_Implementation(ATpsCharacter* const By)
{
	Super::BePickedUp_Implementation(By);
	By->AddPerk(GetClass());
}
