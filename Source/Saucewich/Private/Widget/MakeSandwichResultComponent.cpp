// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/MakeSandwichResultComponent.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/UsersInfo.h"
#include "Names.h"

void UMakeSandwichResultComponent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ResultText = Cast<UTextBlock>(GetWidgetFromName(NAME("Text_Result")));
	MyTeamScoreText = Cast<UTextBlock>(GetWidgetFromName(NAME("Text_MyTeamScore")));
	EnemyTeamScoreText = Cast<UTextBlock>(GetWidgetFromName(NAME("Text_EnemyTeamScore")));
	MyTeamSandwich = Cast<UImage>(GetWidgetFromName(NAME("Image_MyTeamSandwich")));
	EnemyTeamSandwich = Cast<UImage>(GetWidgetFromName(NAME("Image_EnemyTeamSandwich")));
	MyTeamResultImage = Cast<UImage>(GetWidgetFromName(NAME("Image_MyTeamResult")));
	EnemyTeamResultImage = Cast<UImage>(GetWidgetFromName(NAME("Image_EnemyTeamResult")));
	UsersInfo = Cast<UUsersInfo>(GetWidgetFromName(NAME("UserInfo")));
	
	MyTeamSandwichMat =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), SandwichMaterialParent);

	EnemyTeamSandwichMat =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), SandwichMaterialParent);

	MyTeamSandwich->SetBrushFromMaterial(MyTeamSandwichMat);
	EnemyTeamSandwich->SetBrushFromMaterial(EnemyTeamSandwichMat);

	WaitTime = ASaucewichGameMode::GetData(this).NextGameWaitTime;

	FTimerDelegate Delegate;
	Delegate.BindWeakLambda(this, [this] {
		if (--WaitTime <= 0) GetWorld()->GetTimerManager().ClearTimer(WaitTimer);
	});
	GetWorld()->GetTimerManager().SetTimer(WaitTimer, Delegate, 1, true);

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
	ResultText->SetText(ResultTexts.FindRef(ResultName));

	const auto ResultColor = WinningTeam == Invalid ? (MyTeamColor + EnemyTeamColor) * 0.5f : MyTeamColor;
	ResultText->SetColorAndOpacity(ResultColor);

	MyTeamSandwichMat->SetVectorParameterValue(Names::Color, MyTeamColor);
	EnemyTeamSandwichMat->SetVectorParameterValue(Names::Color, EnemyTeamColor);

	MyTeamScoreText->SetText(FText::FromString(FString::FromInt(MyTeamScore)));
	EnemyTeamScoreText->SetText(FText::FromString(FString::FromInt(EnemyTeamScore)));

	MyTeamScoreText->SetColorAndOpacity(FSlateColor(MyTeamColor));
	EnemyTeamScoreText->SetColorAndOpacity(FSlateColor(EnemyTeamColor));

	MyTeamResultImage->SetBrushFromTexture(ResultTextures.FindRef(ResultName));
	EnemyTeamResultImage->SetBrushFromTexture(ResultTextures.FindRef(EnemyResultName));
}
