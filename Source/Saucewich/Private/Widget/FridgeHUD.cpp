// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/FridgeHUD.h"
#include "Player/SaucewichPlayerController.h"
#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"

void UFridgeHUD::Init(const uint8 InFridgeTeam)
{
	FridgeTeam = InFridgeTeam;
	
	const auto PC = CastChecked<ASaucewichPlayerController>(GetOwningPlayer());
	PC->SafePS(FOnPSSpawnedNative::FDelegate::CreateUObject(this, &UFridgeHUD::InitWithPS));
}

void UFridgeHUD::InitWithPS(ASaucewichPlayerState* const InPS)
{
	const auto PS = CastChecked<AMakeSandwichPlayerState>(InPS);
	PS->BindOnTeamChanged(FOnTeamChangedNative::FDelegate::CreateUObject(this, &UFridgeHUD::OnPlayerTeamChanged));
}

void UFridgeHUD::OnPlayerTeamChanged(const uint8 NewTeam)
{
	SetVisibility(NewTeam == FridgeTeam ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}
