// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/AliveWidget.h"

#include "Online/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/FeedBox.h"
#include "Widget/KillFeed.h"
#include "Widget/ScoreFeed.h"

void UAliveWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	KillFeedBox = Cast<UFeedBox>(GetWidgetFromName(TEXT("FeedBox_Kill")));
	ScoreFeedBox = Cast<UFeedBox>(GetWidgetFromName(TEXT("FeedBox_Score")));

	const auto GS = Cast<ASaucewichGameState>(GetWorld()->GetGameState());
	GS->OnPlayerDeath.AddDynamic(this, &UAliveWidget::OnPlayerDeath);

	const auto PC = Cast<ASaucewichPlayerController>(GetOwningPlayer());

	FOnPlayerStateSpawnedSingle PSSpawned;
	PSSpawned.BindDynamic(this, &UAliveWidget::OnPlayerStateSpawned);
	PC->SafePlayerState(PSSpawned);
}

void UAliveWidget::OnPlayerStateSpawned(ASaucewichPlayerState* PlayerState)
{
	PlayerState->OnScoreAdded.AddDynamic(this, &UAliveWidget::OnScoreAdded);
}

void UAliveWidget::OnPlayerDeath(ASaucewichPlayerState* Victim, ASaucewichPlayerState* Attacker, AActor* Inflictor)
{
	KillFeedBox->MakeNewFeed(FKillFeedContent(Victim, Attacker, Inflictor));
}

void UAliveWidget::OnScoreAdded(const FName ScoreName, const int32 ActualScore)
{
	ScoreFeedBox->MakeNewFeed(FScoreFeedContent(ScoreName, ActualScore));
}
