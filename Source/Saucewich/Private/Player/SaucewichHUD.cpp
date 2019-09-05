// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "Online/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	GameState = GetWorld()->GetGameState<ASaucewichGameState>();
	BindChangeColor();
}

void ASaucewichHUD::ChangeColor(const uint8 NewTeam)
{
	const auto MyTeamColor = GameState->GetTeamData(NewTeam).Color;
	OnChangeColor.Broadcast(MyTeamColor);
}

void ASaucewichHUD::BindChangeColor()
{
	const auto PlayerState = GetOwningPlayerController()->GetPlayerState<ASaucewichPlayerState>();

	if (PlayerState)
	{
		PlayerState->OnTeamChangedDelegate.AddDynamic(this, &ASaucewichHUD::ChangeColor);
		ChangeColor(PlayerState->GetTeam());
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ASaucewichHUD::BindChangeColor);
	}
}