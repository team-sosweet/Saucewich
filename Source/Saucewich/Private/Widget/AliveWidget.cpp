// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/AliveWidget.h"

#include "TextBlock.h"
#include "TimerManager.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/FeedBox.h"
#include "Widget/KillFeed.h"
#include "Widget/ScoreFeed.h"
#include "Widget/MessageFeed.h"

void UAliveWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	KillFeedBox = Cast<UFeedBox>(GetWidgetFromName("FeedBox_Kill"));
	ScoreFeedBox = Cast<UFeedBox>(GetWidgetFromName("FeedBox_Score"));
	MessageFeedBox = Cast<UFeedBox>(GetWidgetFromName("FeedBox_Message"));
	CenterText = Cast<UTextBlock>(GetWidgetFromName("CenterText"));

	GameState = Cast<ASaucewichGameState>(GetWorld()->GetGameState());
	GameState->OnPlayerDeath.AddDynamic(this, &UAliveWidget::OnPlayerDeath);

	if (const auto PC = Cast<ASaucewichPlayerController>(GetOwningPlayer()))
	{
		FOnPlayerStateSpawnedSingle PSSpawned;
		PSSpawned.BindDynamic(this, &UAliveWidget::OnPlayerStateSpawned);
		PC->SafePlayerState(PSSpawned);

		PC->OnReceiveMessage.AddDynamic(this, &UAliveWidget::PrintMessage);
	}
}

void UAliveWidget::OnPlayerStateSpawned(ASaucewichPlayerState* PlayerState)
{
	PlayerState->OnScoreAdded.AddDynamic(this, &UAliveWidget::OnScoreAdded);
}

void UAliveWidget::OnPlayerDeath(ASaucewichPlayerState* Victim, ASaucewichPlayerState* Attacker, AActor* Inflictor)
{
	KillFeedBox->MakeNewFeed(FKillFeedContent(Victim, Attacker, Inflictor));
}

void UAliveWidget::OnScoreAdded(const FName ScoreID, const int32 ActualScore)
{
	const auto& DisplayName = GameState->GetScoreData(ScoreID).DisplayName;
	ScoreFeedBox->MakeNewFeed(FScoreFeedContent(DisplayName, ActualScore));
}

void UAliveWidget::PrintMessage(const FText& Message, const float Duration, const EMsgType Type)
{
	switch (Type)
	{
	case EMsgType::Center:
		CenterText->SetText(Message);
		CenterText->SetVisibility(ESlateVisibility::HitTestInvisible);
		GetWorld()->GetTimerManager().SetTimer(CenterTextTimer, this, &UAliveWidget::ClearCenterMessage, Duration);
		break;
	case EMsgType::Left:
		MessageFeedBox->MakeNewFeed(FMessageFeedContent{Message});
		break;
	default: ;
	}
}

void UAliveWidget::ClearCenterMessage() const
{
	CenterText->SetVisibility(ESlateVisibility::Collapsed);
}
