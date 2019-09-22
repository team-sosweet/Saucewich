// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/MakeSandwichResultComponent.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/UsersInfo.h"

void UMakeSandwichResultComponent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ResultText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_Result")));
	MyTeamScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_MyTeamScore")));
	EnemyTeamScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text_EnemyTeamScore")));
	MyTeamSandwich = Cast<UImage>(GetWidgetFromName(TEXT("Image_MyTeamSandwich")));
	EnemyTeamSandwich = Cast<UImage>(GetWidgetFromName(TEXT("Image_EnemyTeamSandwich")));
	MyTeamResultImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_MyTeamResult")));
	EnemyTeamResultImage = Cast<UImage>(GetWidgetFromName(TEXT("Image_EnemyTeamResult")));
	UsersInfo = Cast<UUsersInfo>(GetWidgetFromName(TEXT("UserInfo")));
	
	GameState = GetWorld()->GetGameState<ASaucewichGameState>();

	const auto PC = GetOwningPlayer<ASaucewichPlayerController>();
	
	FOnPlayerStateSpawnedSingle PSSpawned;
	PSSpawned.BindDynamic(this, &UMakeSandwichResultComponent::GetPlayerState);
	PC->SafePlayerState(PSSpawned);
	
	MyTeamSandwichMat =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), SandwichMaterialParent);

	EnemyTeamSandwichMat =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), SandwichMaterialParent);

	MyTeamSandwich->SetBrushFromMaterial(MyTeamSandwichMat);
	EnemyTeamSandwich->SetBrushFromMaterial(EnemyTeamSandwichMat);

	const auto GameModeClass = *GameState->GameModeClass;
	const auto GameMode = GameModeClass->GetDefaultObject<ASaucewichGameMode>();

	WaitTime = GameMode->GetNextGameWaitTime();

	GetWorld()->GetTimerManager().SetTimer(WaitTimer, [this]
		{
			if (--WaitTime <= 0)
			{
				GetWorld()->GetTimerManager().ClearTimer(WaitTimer);
			}
		}, 1.0f, true);

	UsersInfo->UpdateInfo();
}

void UMakeSandwichResultComponent::SetWidget(uint8 WinningTeam)
{
	const auto MyTeam = PlayerState->GetTeam();
	const auto EnemyTeam = MyTeam == 1u ? 2u : 1u;

	const auto MyTeamScore = GameState->GetTeamScore(MyTeam);
	const auto EnemyTeamScore = GameState->GetTeamScore(EnemyTeam);
	
	const auto MyTeamColor = GameState->GetTeamData(MyTeam).Color;
	const auto EnemyTeamColor = GameState->GetTeamData(EnemyTeam).Color;

	const auto ScoreIndex = (WinningTeam == MyTeam ? 1 : WinningTeam == 0u ? 0 : -1) - 1;

	ResultText->SetText(ResultTexts[ScoreIndex]);

	const auto ResultColor = ScoreIndex != 1 ? MyTeamColor : (MyTeamColor + EnemyTeamColor) * 0.5f;
	ResultText->SetColorAndOpacity(FSlateColor(ResultColor));

	MyTeamSandwichMat->SetVectorParameterValue(TEXT("Color"), MyTeamColor);
	EnemyTeamSandwichMat->SetVectorParameterValue(TEXT("Color"), EnemyTeamColor);

	MyTeamScoreText->SetText(FText::FromString(FString::FromInt(MyTeamScore)));
	EnemyTeamScoreText->SetText(FText::FromString(FString::FromInt(EnemyTeamScore)));

	MyTeamScoreText->SetColorAndOpacity(FSlateColor(MyTeamColor));
	EnemyTeamScoreText->SetColorAndOpacity(FSlateColor(EnemyTeamColor));

	MyTeamResultImage->SetBrushFromTexture(ResultTextures[ScoreIndex]);
	EnemyTeamResultImage->SetBrushFromTexture(ResultTextures[-(ScoreIndex - 1) + 1]);
}

void UMakeSandwichResultComponent::GetPlayerState(ASaucewichPlayerState* InPlayerState)
{
	PlayerState = InPlayerState;
}
