// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "TimerManager.h"

#include "Online/SaucewichGameState.h"
#include "SaucewichGameInstance.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/AliveWidget.h"
#include "Widget/DeathWidget.h"
#include "Widget/ResultWidget.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	const auto PC = GetOwningPlayerController();
	
	AliveWidget = CreateWidget<UAliveWidget>(PC);
	DeathWidget = CreateWidget<UDeathWidget>(PC);
	ResultWidget = CreateWidget<UResultWidget>(PC);

	const auto GameRule = GetGameInstance<USaucewichGameInstance>()->GetGameRule();

	AliveWidget->SetComponent(AliveComponentsClass[GameRule]);
	DeathWidget->SetComponent(DeathComponentsClass[GameRule]);
	ResultWidget->SetComponent(ResultComponentsClass[GameRule]);
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