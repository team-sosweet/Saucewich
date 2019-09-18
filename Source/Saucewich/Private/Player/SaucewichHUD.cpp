// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	GameState = GetWorld()->GetGameState<ASaucewichGameState>();
	
	const auto PC = Cast<ASaucewichPlayerController>(GetOwningPlayerController());
	FOnPlayerStateSpawnedSingle PSDelegate;
	PSDelegate.BindDynamic(this, &ASaucewichHUD::OnGetPlayerState);
	PC->SafePlayerState(PSDelegate);
}

void ASaucewichHUD::BindChangedColor(const FOnChangedColorSingle& InDelegate)
{
	OnChangedColor.AddUnique(InDelegate);
	InDelegate.ExecuteIfBound(MyTeamColor);
}

void ASaucewichHUD::OnGetPlayerState(ASaucewichPlayerState* PS)
{
	PS->OnTeamChangedDelegate.AddDynamic(this, &ASaucewichHUD::ChangedColor);
	ChangedColor(PS->GetTeam());
}

void ASaucewichHUD::ChangedColor(const uint8 NewTeam)
{
	MyTeamColor = GameState->GetTeamData(NewTeam).Color;
	OnChangedColor.Broadcast(MyTeamColor);
}