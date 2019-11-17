// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "SaucewichGameMode.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PC = Cast<ASaucewichPlayerController>(GetOwningPlayerController()))
	{
		FOnPlayerStateSpawnedSingle PSDelegate;
		PSDelegate.BindDynamic(this, &ASaucewichHUD::OnGetPlayerState);
		PC->SafePlayerState(PSDelegate);
	}
}

void ASaucewichHUD::BindChangedColor(const FOnChangedColorSingle& InDelegate)
{
	check(InDelegate.IsBound());
	OnChangedColor.AddUnique(InDelegate);

	auto&& Data = ASaucewichGameMode::GetData(this);
	const auto PlayerState = CastChecked<ASaucewichPlayerState>(GetOwningPlayerController()->PlayerState);
	(void)InDelegate.Execute(Data.Teams[PlayerState->GetTeam()].Color);
}

void ASaucewichHUD::OnGetPlayerState(ASaucewichPlayerState* const PS)
{
	PS->OnTeamChangedDelegate.AddDynamic(this, &ASaucewichHUD::ChangedColor);
	ChangedColor(PS->GetTeam());
}

void ASaucewichHUD::ChangedColor(const uint8 NewTeam)
{
	OnChangedColor.Broadcast(ASaucewichGameMode::GetData(this).Teams[NewTeam].Color);
}
