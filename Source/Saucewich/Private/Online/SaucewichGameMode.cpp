// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameMode.h"
#include "GameFramework/OnlineSession.h"
#include "Kismet/GameplayStatics.h"
#include "SaucewichGameState.h"
#include "SaucewichPlayerState.h"
#include "TpsCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichGameMode, Log, All)

void ASaucewichGameMode::BeginPlay()
{
	Super::BeginPlay();
	State = GetGameState<ASaucewichGameState>();
	if (!State)
	{
		UE_LOG(LogSaucewichGameMode, Error, TEXT("GameState가 유효하지 않습니다. GameMode가 비활성화 됩니다. Class가 SaucewichGameState가 맞는지 확인해주세요."));
		return;
	}
}

void ASaucewichGameMode::SetPlayerDefaults(APawn* const PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	if (const auto Character = Cast<ATpsCharacter>(PlayerPawn))
	{
		GiveWeapons(Character);
	}
}

FString ASaucewichGameMode::InitNewPlayer(APlayerController* PC, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	const auto ErrorMessage = Super::InitNewPlayer(PC, UniqueId, Options, Portal);
	if (!State) return ErrorMessage;

	auto Team = UGameplayStatics::GetIntOption(Options, "Team", 0);
	if (Team == 0) Team = State->GetMinPlayerTeam();
	PC->GetPlayerState<ASaucewichPlayerState>()->SetTeam(Team);

	return ErrorMessage;
}
