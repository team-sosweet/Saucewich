// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameMode.h"
#include "Player/SaucewichPlayerState.h"
#include "Player/SaucewichPlayerController.h"

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
	check(!OnChangedColor.Contains(InDelegate));
	OnChangedColor.Add(InDelegate);

	auto&& Data = ASaucewichGameMode::GetData(this);

	const auto PS = GetOwningPlayerController()->PlayerState;
	if (PS) InDelegate.Execute(Data.Teams[CastChecked<ASaucewichPlayerState>(PS)->GetTeam()].Color);
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
