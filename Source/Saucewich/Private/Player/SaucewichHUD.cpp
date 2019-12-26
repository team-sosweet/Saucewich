// Copyright 2019 Othereum. All Rights Reserved.

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
	OnChangedColor.AddUnique(InDelegate);

	auto&& Data = ASaucewichGameMode::GetData(this);
	
	if (const auto PlayerState = CastChecked<ASaucewichPlayerState>(GetOwningPlayerController()->PlayerState, ECastCheckedType::NullAllowed))
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
