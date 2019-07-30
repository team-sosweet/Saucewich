// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"
#include "UnrealNetwork.h"

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	Team = NewTeam;
	OnTeamChanged();
}

void ASaucewichPlayerState::OnTeamChanged() const
{
	OnTeamChangedDelegate.Broadcast(Team);
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
}
