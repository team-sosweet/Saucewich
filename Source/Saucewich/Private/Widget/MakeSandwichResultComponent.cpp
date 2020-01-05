// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/MakeSandwichResultComponent.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/UsersInfo.h"
#include "Names.h"

void UMakeSandwichResultComponent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ResultText = CastChecked<UTextBlock>(GetWidgetFromName(NAME("Text_Result")));
	MyTeamScoreText = CastChecked<UTextBlock>(GetWidgetFromName(NAME("Text_MyTeamScore")));
	EnemyTeamScoreText = CastChecked<UTextBlock>(GetWidgetFromName(NAME("Text_EnemyTeamScore")));
	MyTeamSandwich = CastChecked<UImage>(GetWidgetFromName(NAME("Image_MyTeamSandwich")));
	EnemyTeamSandwich = CastChecked<UImage>(GetWidgetFromName(NAME("Image_EnemyTeamSandwich")));
	MyTeamResultImage = CastChecked<UImage>(GetWidgetFromName(NAME("Image_MyTeamResult")));
	EnemyTeamResultImage = CastChecked<UImage>(GetWidgetFromName(NAME("Image_EnemyTeamResult")));
	UsersInfo = CastChecked<UUsersInfo>(GetWidgetFromName(NAME("UserInfo")));

	const auto World = GetWorld();
	
	MyTeamSandwichMat =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(World, SandwichMaterialParent);

	EnemyTeamSandwichMat =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(World, SandwichMaterialParent);

	MyTeamSandwich->SetBrushFromMaterial(MyTeamSandwichMat);
	EnemyTeamSandwich->SetBrushFromMaterial(EnemyTeamSandwichMat);

	NextMatchStartTime = World->GetRealTimeSeconds() + ASaucewichGameMode::GetData(this).NextGameWaitTime;

	UsersInfo->UpdateInfo();
}

void UMakeSandwichResultComponent::SetWidget(const uint8 WinningTeam) const
{
	const auto MyTeam = GetOwningPlayerState<ASaucewichPlayerState>(true)->GetTeam();
	const uint8 EnemyTeam = 1 - MyTeam;

	const auto GameState = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	auto&& TeamData = ASaucewichGameMode::GetData(this).Teams;
	
	const auto MyTeamScore = GameState->GetTeamScore(MyTeam);
	const auto EnemyTeamScore = GameState->GetTeamScore(EnemyTeam);
	
	auto&& MyTeamColor = TeamData[MyTeam].Color;
	auto&& EnemyTeamColor = TeamData[EnemyTeam].Color;

	constexpr uint8 Invalid = -1;
	const auto ResultName = WinningTeam == MyTeam ? Names::Win : WinningTeam == Invalid ? Names::Draw : Names::Lose;
	const auto EnemyResultName = WinningTeam == MyTeam ? Names::Lose : WinningTeam == Invalid ? Names::Draw : Names::Win;
	ResultText->SetText(ResultTexts[ResultName]);

	const auto ResultColor = WinningTeam == Invalid ? (MyTeamColor + EnemyTeamColor) * 0.5f : MyTeamColor;
	ResultText->SetColorAndOpacity(ResultColor);

	MyTeamSandwichMat->SetVectorParameterValue(Names::Color, MyTeamColor);
	EnemyTeamSandwichMat->SetVectorParameterValue(Names::Color, EnemyTeamColor);

	MyTeamScoreText->SetText(FText::FromString(FString::FromInt(MyTeamScore)));
	EnemyTeamScoreText->SetText(FText::FromString(FString::FromInt(EnemyTeamScore)));

	MyTeamScoreText->SetColorAndOpacity(MyTeamColor);
	EnemyTeamScoreText->SetColorAndOpacity(EnemyTeamColor);

	MyTeamResultImage->SetBrushFromTexture(ResultTextures[ResultName]);
	EnemyTeamResultImage->SetBrushFromTexture(ResultTextures[EnemyResultName]);
}

float UMakeSandwichResultComponent::GetTimeRemainingForNextGame() const
{
	return FMath::Max(NextMatchStartTime - GetWorld()->GetRealTimeSeconds(), 0.f);
}
