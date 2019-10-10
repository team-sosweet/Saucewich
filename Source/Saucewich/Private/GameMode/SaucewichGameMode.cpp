// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameMode.h"

#include "Engine/Engine.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TimerManager.h"

#include "Entity/PickupSpawner.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacter.h"
#include "SaucewichGameInstance.h"
#include "SaucewichLibrary.h"
#include "JsonData.h"

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
	for (const auto PC : TActorRange<ASaucewichPlayerController>{GetWorld()})
	{
		PC->PrintMessage(MessageID, Duration);
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

FString ASaucewichGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	check(NewPlayerController);

	if (!NewPlayerController->PlayerState)
		return "PlayerState is null";

	GameSession->RegisterPlayer(NewPlayerController, UniqueId.GetUniqueNetId(), false);

	
	auto InName = UGameplayStatics::ParseOption(Options, "Name");
	
	if (!USaucewichLibrary::IsValidPlayerName(InName))
		InName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), NewPlayerController->PlayerState->PlayerId);
	
	ChangeName(NewPlayerController, InName, false);

	
	return {};
}

void ASaucewichGameMode::PostLogin(APlayerController* const NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ExtUpdatePlyCnt();
}

void ASaucewichGameMode::Logout(AController* const Exiting)
{
	Super::Logout(Exiting);
	ExtUpdatePlyCnt();
}

void ASaucewichGameMode::HandleStartingNewPlayer_Implementation(APlayerController* const NewPlayer)
{
}

void ASaucewichGameMode::GenericPlayerInitialization(AController* const C)
{
	Super::GenericPlayerInitialization(C);
	
	if (const auto GS = GetGameState<ASaucewichGameState>())
	{
		if (const auto PS = C->GetPlayerState<ASaucewichPlayerState>())
		{
			const auto MinTeamNum = GS->GetNumPlayers(GS->GetMinPlayerTeam());
			const auto MyTeamNum = GS->GetNumPlayers(PS->GetTeam());
			if (PS->GetTeam() == 0 || MyTeamNum - MinTeamNum > 1)
			{
				PS->SetTeam(GS->GetMinPlayerTeam());
			}
		}
	}
}

bool ASaucewichGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
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

	for (const auto PlayerStart : TActorRange<APlayerStart>{GetWorld()})
	{
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
	return NumPlayers >= MinPlayerToStart;
}

bool ASaucewichGameMode::ReadyToEndMatch_Implementation()
{
	if (const auto GS = GetGameState<ASaucewichGameState>())
	{
		if (GS->GetRemainingRoundSeconds() <= 0)
		{
			return true;
		}

		if (NumPlayers == 0)
		{
			return true;
		}
	}
	
	return false;
}

void ASaucewichGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	ExtUpdatePlyCnt();
}

void ASaucewichGameMode::HandleMatchHasStarted()
{
	GameSession->HandleMatchHasStarted();

	GEngine->BlockTillLevelStreamingCompleted(GetWorld());
	GetWorldSettings()->NotifyBeginPlay();
	GetWorldSettings()->NotifyMatchStarted();

	for (const auto Character : TActorRange<ATpsCharacter>{GetWorld()})
	{
		Character->KillSilent();
	}

	for (const auto Spawner : TActorRange<APickupSpawner>{GetWorld()})
	{
		Spawner->SetSpawnTimer();
	}
}

void ASaucewichGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	
	if (NumPlayers == 0)
	{
		StartNextGame();
	}
	else
	{
		if (const auto GI = GetGameInstance<USaucewichGameInstance>())
			GI->GetGameCode([this](const FString& GameCode){ExtUpdatePlyCnt(GameCode, MaxPlayers);});

		GetWorldTimerManager().SetTimer(MatchStateTimer, this, &ASaucewichGameMode::StartNextGame, NextGameWaitTime);
	}
}

void ASaucewichGameMode::UpdateMatchState()
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (ReadyToStartMatch())
		{
			const auto bTimerExists = GetWorldTimerManager().TimerExists(MatchStateTimer);
			if (bAboutToStartMatch && !bTimerExists)
			{
				UE_LOG(LogGameMode, Log, TEXT("GameMode returned ReadyToStartMatch"));
				StartMatch();
			}
			else if (!bAboutToStartMatch && !bTimerExists)
			{
				bAboutToStartMatch = true;
				GetWorldTimerManager().SetTimer(MatchStateTimer, MatchStartingTime, false);
				PrintMessage("StartingMatch", MatchStartingTime);
			}
		}
		else
		{
			bAboutToStartMatch = false;
			GetWorldTimerManager().ClearTimer(MatchStateTimer);
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
	
	const auto GmClass = GameModes.Num() > 0 ? GameModes[FMath::RandHelper(GameModes.Num())] : TSubclassOf<ASaucewichGameMode>{GetClass()};
	const auto DefGm = GmClass.GetDefaultObject();

	const TSoftObjectPtr<UWorld> CurMap{GetWorld()->GetPathName()};
	auto AvailableMaps = DefGm->Maps;
	AvailableMaps.RemoveSingleSwap(CurMap, false);
	const auto NewMap = AvailableMaps.Num() > 0 ? AvailableMaps[FMath::RandHelper(AvailableMaps.Num())].GetAssetName() : CurMap.GetAssetName();

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap, *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}

void ASaucewichGameMode::ExtUpdatePlyCnt() const
{
#if !WITH_EDITOR
	if (!IsValidLowLevel()) return;
	if (!IsRunningDedicatedServer()) return;

	if (const auto GI = GetGameInstance<USaucewichGameInstance>())
	{
		GI->GetGameCode([this](const FString& GameCode){ExtUpdatePlyCnt(GameCode, NumPlayers);});
	}
#endif
}

void ASaucewichGameMode::ExtUpdatePlyCnt(const FString& GameCode, const int32 NewCnt) const
{
#if !WITH_EDITOR
	if (!IsValidLowLevel()) return;
	if (!IsRunningDedicatedServer()) return;

	FJson Body;
	Body.Data.Add(TEXT("people"), UJsonData::MakeIntegerData(NewCnt));

	FOnResponded OnResponded;
	OnResponded.BindDynamic(this, &ASaucewichGameMode::RespondExtUpdatePlyCnt);

	GetGameInstance<UHttpGameInstance>()->PutRequest("room/people/" + GameCode, {}, Body, OnResponded);
	UE_LOG(LogExternalServer, Log, TEXT("Updating player count to %d..."), NewCnt);
#endif
}

void ASaucewichGameMode::RespondExtUpdatePlyCnt(const bool bIsSuccess, const int32 Code, FJson Json)
{
	if (bIsSuccess)
	{
		UE_LOG(LogExternalServer, Log, TEXT("Player count update successful"));
		return;
	}
	
	if (Code == 429)
	{
		constexpr float RetryRate = 1;
		GetWorldTimerManager().SetTimer(ExtPlyCntUpdateTimer, this, &ASaucewichGameMode::ExtUpdatePlyCnt, RetryRate);
		UE_LOG(LogExternalServer, Warning, TEXT("Requested player count update too frequently! Retrying in %f seconds..."), RetryRate);
	}
	else
	{
		UE_LOG(LogExternalServer, Error, TEXT("Failed to update player count of external server. Error code: %d"), Code);
	}
}
