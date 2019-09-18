// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/UserInfo.h"

#include "Components/TextBlock.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"

void UUserInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameState = GetWorld()->GetGameState<ASaucewichGameState>();

	NameText = Cast<UTextBlock>(GetWidgetFromName("Text_Name"));
	ScoreText = Cast<UTextBlock>(GetWidgetFromName("Text_Score"));
	ObjectiveText = Cast<UTextBlock>(GetWidgetFromName("Text_Objective"));
	KillText = Cast<UTextBlock>(GetWidgetFromName("Text_Kill"));
	DeathText = Cast<UTextBlock>(GetWidgetFromName("Text_Death"));
}

void UUserInfo::UpdateInfo(ASaucewichPlayerState* PlayerState)
{
	const auto Color = GameState->GetTeamData(PlayerState->GetTeam()).Color;

	const auto Name = PlayerState->GetPlayerName();
	NameText->SetText(FText::FromString(Name));
	NameText->SetColorAndOpacity(Color);

	const auto Score = FString::SanitizeFloat(PlayerState->Score, 0);
	ScoreText->SetText(FText::FromString(Score));
	ScoreText->SetColorAndOpacity(Color);
	
	const auto Objective = FString::FromInt(PlayerState->GetObjective());
	ObjectiveText->SetText(FText::FromString(Objective));
	ObjectiveText->SetColorAndOpacity(Color);

	const auto Kill = FString::FromInt(PlayerState->GetKill());
	KillText->SetText(FText::FromString(Kill));
	KillText->SetColorAndOpacity(Color);

	const auto Death = FString::FromInt(PlayerState->GetDeath());
	DeathText->SetText(FText::FromString(Death));
	DeathText->SetColorAndOpacity(Color);
}
