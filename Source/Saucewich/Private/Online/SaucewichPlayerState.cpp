// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SaucewichPlayerState.h"
#include "UnrealNetwork.h"

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASaucewichPlayerState, Team);
}
