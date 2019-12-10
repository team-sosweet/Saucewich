// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/AliveWidget.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/FeedBox.h"
#include "Widget/KillFeed.h"
#include "Widget/ScoreFeed.h"
#include "Widget/MessageFeed.h"
#include "SaucewichInstance.h"

void UAliveWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	KillFeedBox = CastChecked<UFeedBox>(GetWidgetFromName(TEXT("FeedBox_Kill")));
	ScoreFeedBox = CastChecked<UFeedBox>(GetWidgetFromName(TEXT("FeedBox_Score")));
	MessageFeedBox = CastChecked<UFeedBox>(GetWidgetFromName(TEXT("FeedBox_Message")));
	CenterText = CastChecked<UTextBlock>(GetWidgetFromName(TEXT("CenterText")));

	GameState = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	GameState->OnPlayerDeath.AddDynamic(this, &UAliveWidget::OnPlayerDeath);

	const auto PC = CastChecked<ASaucewichPlayerController>(GetOwningPlayer());
	
	FOnPlayerStateSpawnedSingle PSSpawned;
	PSSpawned.BindDynamic(this, &UAliveWidget::OnPlayerStateSpawned);
	PC->SafePlayerState(PSSpawned);

	PC->OnReceiveMessage.AddDynamic(this, &UAliveWidget::PrintMessage);
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
	const auto GI = GetWorld()->GetGameInstanceChecked<USaucewichInstance>();
	const auto& DisplayName = GI->GetScoreData(ScoreID).DisplayName;
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
