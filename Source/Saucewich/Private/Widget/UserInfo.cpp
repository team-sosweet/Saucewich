// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/UserInfo.h"

#include "Components/TextBlock.h"

#include "Player/SaucewichPlayerState.h"
#include "GameMode/SaucewichGameMode.h"

void UUserInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	NameText = CastChecked<UTextBlock>(GetWidgetFromName(TEXT("Text_Name")));
	ScoreText = CastChecked<UTextBlock>(GetWidgetFromName(TEXT("Text_Score")));
	ObjectiveText = CastChecked<UTextBlock>(GetWidgetFromName(TEXT("Text_Objective")));
	KillText = CastChecked<UTextBlock>(GetWidgetFromName(TEXT("Text_Kill")));
	DeathText = CastChecked<UTextBlock>(GetWidgetFromName(TEXT("Text_Death")));
}

void UUserInfo::UpdateInfo(ASaucewichPlayerState* PlayerState) const
{
	auto&& Color = ASaucewichGameMode::GetData(this).Teams[PlayerState->GetTeam()].Color;

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
