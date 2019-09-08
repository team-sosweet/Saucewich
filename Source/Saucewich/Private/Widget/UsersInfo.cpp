// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/UsersInfo.h"

#include "Online/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"
#include "Widget/UserInfo.h"

void UUsersInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	GameState = GetWorld()->GetGameState<ASaucewichGameState>();

	for (auto Index = 0; Index < 8; Index++)
	{
		const auto WidgetName = FString::Printf(TEXT("UserInfo_%d"), Index);
		const auto UserInfo = Cast<UUserInfo>(GetWidgetFromName(FName(*WidgetName)));
		UserInfos.Add(UserInfo);
	}
}

void UUsersInfo::UpdateInfo()
{
	auto PlayerArray = GameState->PlayerArray;
	TArray<ASaucewichPlayerState*> Players;
	Players.SetNum(PlayerArray.Num());
	
	for (auto Index = 0; Index < Players.Num(); Index++)
	{
		Players[Index] = Cast<ASaucewichPlayerState>(PlayerArray[Index]);
	}

	Players.Sort([](const auto& Lhs, const auto& Rhs)
		{
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
		UserInfos[Index]->UpdateInfo(Players[Index]);
		UserInfos[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	for (; Index < UserInfos.Num(); Index++)
	{
		UserInfos[Index]->SetVisibility(ESlateVisibility::Collapsed);
	}
}
