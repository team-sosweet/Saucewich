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
		if (!IsValidLowLevel()) return;
		if (--WaitTime <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(WaitTimer);
		}
	}, 1.0f, true);

	UsersInfo->UpdateInfo();
}

void UMakeSandwichResultComponent::SetWidget(const uint8 WinningTeam)
{
	const auto MyTeam = PlayerState->GetTeam();
	const uint8 EnemyTeam = MyTeam == 1 ? 2 : 1;

	const auto MyTeamScore = GameState->GetTeamScore(MyTeam);
	const auto EnemyTeamScore = GameState->GetTeamScore(EnemyTeam);
	
	const auto MyTeamColor = GameState->GetTeamData(MyTeam).Color;
	const auto EnemyTeamColor = GameState->GetTeamData(EnemyTeam).Color;

	const auto ResultName = WinningTeam == MyTeam ? "Win" : WinningTeam == 0 ? "Draw" : "Lose";
	const auto EnemyResultName = WinningTeam == MyTeam ? "Lose" : WinningTeam == 0 ? "Draw" : "Win";
	ResultText->SetText(ResultTexts.FindRef(ResultName));

	const auto ResultColor = WinningTeam == 0 ? (MyTeamColor + EnemyTeamColor) * 0.5f : MyTeamColor;
	ResultText->SetColorAndOpacity(FSlateColor(ResultColor));

	MyTeamSandwichMat->SetVectorParameterValue(TEXT("Color"), MyTeamColor);
	EnemyTeamSandwichMat->SetVectorParameterValue(TEXT("Color"), EnemyTeamColor);

	MyTeamScoreText->SetText(FText::FromString(FString::FromInt(MyTeamScore)));
	EnemyTeamScoreText->SetText(FText::FromString(FString::FromInt(EnemyTeamScore)));

	MyTeamScoreText->SetColorAndOpacity(FSlateColor(MyTeamColor));
	EnemyTeamScoreText->SetColorAndOpacity(FSlateColor(EnemyTeamColor));

	MyTeamResultImage->SetBrushFromTexture(ResultTextures.FindRef(ResultName));
	EnemyTeamResultImage->SetBrushFromTexture(ResultTextures.FindRef(EnemyResultName));
}

void UMakeSandwichResultComponent::GetPlayerState(ASaucewichPlayerState* InPlayerState)
{
	PlayerState = InPlayerState;
}
