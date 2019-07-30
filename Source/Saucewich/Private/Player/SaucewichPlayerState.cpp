// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "SaucewichPlayerController.h"

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	Team = NewTeam;
	OnRep_Team();
}

void ASaucewichPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (const auto Controller = Cast<ASaucewichPlayerController>(GetOwner()))
	{
		Controller->OnPlayerStateSpawned.Broadcast(this);
	}
}

void ASaucewichPlayerState::OnRep_Team()
{
	OnTeamChanged.Broadcast();
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
}
