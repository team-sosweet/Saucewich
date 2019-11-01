// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameMode.h"

#include "GameLiftServerSDK.h"

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
#include "Saucewich.h"

ASaucewichGameMode::ASaucewichGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseSeamlessTravel = true;
}

void ASaucewichGameMode::SetPlayerRespawnTimer(ASaucewichPlayerController* const PC) const
{
	PC->SetRespawnTimer(MinRespawnDelay);
}

void ASaucewichGameMode::PrintMessage(const FText& Message, const EMsgType Type, const float Duration) const
{
	for (const auto PC : TActorRange<ASaucewichPlayerController>{GetWorld()})
	{
		PC->PrintMessage(Message, Duration, Type);
	}
}

const FText& ASaucewichGameMode::GetMessage(const FName ID) const
{
	if (const auto Found = Messages.Find(ID))
		return *Found;

	UE_LOG(LogGameMode, Error, TEXT("메시지 ID '%s'에 대한 메시지 텍스트가 없습니다!"), *ID.ToString());
	return FText::GetEmpty();
}

TSubclassOf<ASaucewichGameMode> ASaucewichGameMode::ChooseNextGameMode() const
{
	return GameModes.Num() > 0 ? GameModes[FMath::RandHelper(GameModes.Num())] : GetClass();
}

TSoftObjectPtr<UWorld> ASaucewichGameMode::ChooseNextMap() const
{
	const TSoftObjectPtr<UWorld> CurMap = GetWorld();
	auto NextMaps = Maps;
	NextMaps.RemoveSingleSwap(CurMap, false);
	return NextMaps.Num() > 0 ? NextMaps[FMath::RandHelper(NextMaps.Num())] : CurMap;
}

void ASaucewichGameMode::OnPlayerChangedName(ASaucewichPlayerState* const Player, FString&& OldName)
{
	PrintAndLogFmtMsg("NameChange", FText::FromString(MoveTemp(OldName)), FText::FromString(Player->GetPlayerName()));
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

void ASaucewichGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!ErrorMessage.IsEmpty()) return;

#if !WITH_EDITOR
	const auto ClVer = FCString::Atoi(*UGameplayStatics::ParseOption(Options, "ServerVersion"));
	const auto SvVer = USaucewich::GetServerVersion();
	if (ClVer != SvVer)
	{
		ErrorMessage = FString::Printf(TEXT("Version mismatch. Client: %d, Server: %d"), ClVer, SvVer);
		return;
	}
#endif
	
#if WITH_GAMELIFT
	auto& GameLiftSdkModule = USaucewich::GetGameLiftServerSDKModule();
	const auto Result = GameLiftSdkModule.AcceptPlayerSession(UGameplayStatics::ParseOption(Options, "SessionID"));
	if (!Result.IsSuccess())
	{
		ErrorMessage = Result.GetError().m_errorMessage;
		return;
	}
#endif
}

FString ASaucewichGameMode::InitNewPlayer(APlayerController* const NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	const auto PC = CastChecked<ASaucewichPlayerController>(NewPlayerController);

	if (!PC->PlayerState)
		return "PlayerState is null";

	GameSession->RegisterPlayer(PC, UniqueId.GetUniqueNetId(), false);

	
	auto PlayerName = UGameplayStatics::ParseOption(Options, "PlayerName");
	
	if (USaucewich::IsValidPlayerName(PlayerName) != ENameValidity::Valid)
		PlayerName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), PC->PlayerState->PlayerId);
	
	ChangeName(PC, PlayerName, false);


	PC->SetSessionID(UGameplayStatics::ParseOption(Options, "SessionID"));
	
	
	return {};
}

void ASaucewichGameMode::PostLogin(APlayerController* const NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PrintAndLogFmtMsg("Login", FText::FromString(NewPlayer->PlayerState->GetPlayerName()));
}

void ASaucewichGameMode::Logout(AController* const Exiting)
{
	Super::Logout(Exiting);
	PrintAndLogFmtMsg("Logout", FText::FromString(Exiting->PlayerState->GetPlayerName()));

#if WITH_GAMELIFT
	if (const auto PC = Cast<ASaucewichPlayerController>(Exiting))
	{
		auto& Module = USaucewich::GetGameLiftServerSDKModule();
		Module.RemovePlayerSession(PC->GetSessionID());
	}
#endif
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

	PrintAndLogFmtMsg("MatchStart");
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
				PrintMessage(GetMessage("StartingMatch"), EMsgType::Center, MatchStartingTime);
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
	const auto GmClass = ChooseNextGameMode();
	const auto DefGm = GmClass.GetDefaultObject();
	const auto NewMap = DefGm->ChooseNextMap();

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap.GetAssetName(), *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}

void ASaucewichGameMode::PrintAndLogFmtMsg(const FName MsgID) const
{
	auto&& Msg = GetMessage(MsgID);
	PrintMessage(Msg, EMsgType::Center);
	UE_LOG(LogGameMode, Log, TEXT("%s"), *Msg.ToString());
}
