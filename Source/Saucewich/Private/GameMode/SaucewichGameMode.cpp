// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameMode.h"

#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"
#include "SaucewichGameInstance.h"

ASaucewichGameMode::ASaucewichGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseSeamlessTravel = true;
}

void ASaucewichGameMode::SetPlayerRespawnTimer(ASaucewichPlayerController* const PC) const
{
	PC->SetRespawnTimer(MinRespawnDelay);
}

void ASaucewichGameMode::PrintMessage(const FName MessageID, const float Duration) const
{
	for (TActorIterator<ASaucewichPlayerController> It{GetWorld()}; It; ++It)
	{
		It->PrintMessage(MessageID, Duration);
	}
}

void ASaucewichGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (GameSession) GameSession->MaxPlayers = MaxPlayers;
}

void ASaucewichGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(MatchStateUpdateTimer, this, &ASaucewichGameMode::UpdateMatchState, MatchStateUpdateInterval, true);
}

void ASaucewichGameMode::GenericPlayerInitialization(AController* const C)
{
	Super::GenericPlayerInitialization(C);
	
	if (const auto GS = GetGameState<ASaucewichGameState>())
		if (const auto PS = C->GetPlayerState<ASaucewichPlayerState>())
			if (PS->GetTeam() == 0 || GS->GetNumPlayers(PS->GetTeam()) > GameSession->MaxPlayers / GS->GetNumTeam())
				PS->SetTeam(GS->GetMinPlayerTeam());
}

AActor* ASaucewichGameMode::ChoosePlayerStart_Implementation(AController* const Player)
{
	const auto PawnClass = GetDefaultPawnClassForController(Player);
	const auto PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	const auto World = GetWorld();

	TCHAR Tag[] = TEXT("0");
	Tag[0] = TEXT('0') + Player->GetPlayerState<ASaucewichPlayerState>()->GetTeam();

	// 4개의 TArray는 우선순위를 나타내며 0번이 가장 높다.
	TArray<APlayerStart*> StartPoints[4];

	for (TActorIterator<APlayerStart> It{World}; It; ++It)
	{
		const auto PlayerStart = *It;

#if WITH_EDITOR
		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			return PlayerStart;
		}
#endif

		const auto bMyTeam = PlayerStart->PlayerStartTag == Tag;
		auto ActorLocation = PlayerStart->GetActorLocation();
		const auto ActorRotation = PlayerStart->GetActorRotation();
		if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
		{
			(bMyTeam ? StartPoints[0] : StartPoints[2]).Add(PlayerStart);
		}
		else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
		{
			(bMyTeam ? StartPoints[1] : StartPoints[3]).Add(PlayerStart);
		}
	}

	for (const auto& Starts : StartPoints)
	{
		if (Starts.Num() > 0)
		{
			return Starts[FMath::RandHelper(Starts.Num())];
		}
	}

	return nullptr;
}

void ASaucewichGameMode::RestartPlayerAtPlayerStart(AController* const NewPlayer, AActor* const StartSpot)
{
	if (!IsValid(NewPlayer)) return;
	if (!StartSpot) return;
	if (MustSpectate(Cast<APlayerController>(NewPlayer))) return;

	if (!NewPlayer->GetPawn() && GetDefaultPawnClassForController(NewPlayer))
	{
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, StartSpot));
	}

	if (!NewPlayer->GetPawn())
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		InitStartSpot(StartSpot, NewPlayer);
		auto& Transform = StartSpot->GetRootComponent()->GetComponentTransform();
		NewPlayer->GetPawn()->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
		FinishRestartPlayer(NewPlayer, StartSpot->GetActorRotation());
	}
}

void ASaucewichGameMode::SetPlayerDefaults(APawn* const PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	if (const auto PS = PlayerPawn->GetPlayerState<ASaucewichPlayerState>())
	{
		PS->GiveWeapons();
	}
}

bool ASaucewichGameMode::ReadyToStartMatch_Implementation()
{
	return NumPlayers + NumBots >= MinPlayerToStart;
}

bool ASaucewichGameMode::ReadyToEndMatch_Implementation()
{
	if (const auto GS = GetGameState<ASaucewichGameState>())
	{
		if (GS->GetRemainingRoundSeconds() <= 0)
		{
			return true;
		}

		if (GS->GetEmptyTeam() != 0)
		{
			return true;
		}
	}
	
	return false;
}

void ASaucewichGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	GetWorldTimerManager().SetTimer(NextGameTimer, this, &ASaucewichGameMode::StartNextGame, NextGameWaitTime);
}

void ASaucewichGameMode::UpdateMatchState()
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (ReadyToStartMatch())
		{
			UE_LOG(LogGameMode, Log, TEXT("GameMode returned ReadyToStartMatch"));
			StartMatch();
		}
	}
	if (GetMatchState() == MatchState::InProgress)
	{
		if (ReadyToEndMatch())
		{
			UE_LOG(LogGameMode, Log, TEXT("GameMode returned ReadyToEndMatch"));
			EndMatch();
		}
	}
}

void ASaucewichGameMode::StartNextGame() const
{
	auto& GameModes = GetGameInstance<USaucewichGameInstance>()->GetGameModes();
	const TSubclassOf<ASaucewichGameMode> GmClass = GameModes.Num() > 0 ? GameModes[FMath::RandHelper(GameModes.Num())] : GetClass();
	const auto DefGm = GmClass.GetDefaultObject();

	const TSoftObjectPtr<UWorld> CurMap{GetWorld()->GetPathName()};
	auto AvailableMaps = DefGm->Maps;
	AvailableMaps.RemoveSingleSwap(CurMap, false);
	const auto NewMap = AvailableMaps.Num() > 0 ? AvailableMaps[FMath::RandHelper(AvailableMaps.Num())].GetAssetName() : CurMap.GetAssetName();

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap, *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}
