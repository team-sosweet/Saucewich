// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/UsersInfo.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/UserInfo.h"

void UUsersInfo::UpdateInfo()
{
	auto Players = GameState->PlayerArray;

	Players.Sort([](APlayerState& A, APlayerState& B)
	{
		auto&& Lhs = *CastChecked<ASaucewichPlayerState>(&A);
		auto&& Rhs = *CastChecked<ASaucewichPlayerState>(&B);
		
		if (Lhs.Score != Rhs.Score)
		{
			return Lhs.Score > Rhs.Score;
		}
		if (Lhs.GetObjective() != Rhs.GetObjective())
		{
			return Lhs.GetObjective() > Rhs.GetObjective();
		}
		if (Lhs.GetKill() != Rhs.GetKill())
		{
			return Lhs.GetKill() > Rhs.GetKill();
		}
		if (Lhs.GetDeath() != Rhs.GetDeath())
		{
			return Lhs.GetDeath() < Rhs.GetDeath();
		}
		return false;
	});

	auto Index = 0;
	
	for (; Index < Players.Num(); Index++)
	{
		UserInfos[Index]->UpdateInfo(CastChecked<ASaucewichPlayerState>(Players[Index]));
		UserInfos[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	for (; Index < UserInfos.Num(); Index++)
	{
		UserInfos[Index]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUsersInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	GameState = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());

	for (auto Index = 0; ; Index++)
	{
		FString WidgetName = TEXT("UserInfo_");
		WidgetName.AppendInt(Index);
		const auto UserInfo = GetWidgetFromName(*WidgetName);

		if (!UserInfo) break;
		
		UserInfos.Add(CastChecked<UUserInfo>(UserInfo));
	}
}

void UUsersInfo::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateInfo();
}
