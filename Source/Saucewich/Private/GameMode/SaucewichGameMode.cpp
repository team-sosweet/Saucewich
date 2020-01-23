// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "GameMode/SaucewichGameMode.h"

#if WITH_GAMELIFT
	#include "GameLiftServerSDK.h"
#endif

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
#include "SaucewichInstance.h"
#include "Names.h"

#define LOCTEXT_NAMESPACE ""

namespace MatchState
{
	const FName Ending = TEXT("Ending");
}

const FGameData& ASaucewichGameMode::GetData(const UObject* WorldContextObj)
{
	return CastChecked<ASaucewichGameMode>(WorldContextObj->GetWorld()->GetGameState()->GetDefaultGameMode())->Data;
}

ASaucewichGameMode::ASaucewichGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseSeamlessTravel = true;
}

void ASaucewichGameMode::SetPlayerRespawnTimer(ASaucewichPlayerController* const PC) const
{
	PC->SetRespawnTimer(MinRespawnDelay);
}

void ASaucewichGameMode::PrintMessage(const FText& Msg, const EMsgType Type, const float Duration)
{
	if (Msg.IsEmpty()) return;
	
	for (const auto PC : TActorRange<ASaucewichPlayerController>{GetWorld()})
	{
		PC->PrintMessage(Msg, Duration, Type);
	}
	UE_LOG(LogGameMode, Log, TEXT("%s"), *Msg.ToString());
}

template <class T>
static T RandomDistinct(TArray<T> Arr, const UObject* const Elem)
{
	auto Predicate = [&](const T& E)
	{
		return E.GetAssetName() == Elem->GetName();
	};
	
	const auto Found = Arr.FindByPredicate(Predicate);
	if (Found) Arr.RemoveAtSwap(Found - Arr.GetData());
	
	return Arr.Num() > 0 ? Arr[FMath::RandHelper(Arr.Num())] : T{Elem};
}

TSoftClassPtr<ASaucewichGameMode> ASaucewichGameMode::ChooseNextGameMode() const
{
	const TSoftClassPtr<ASaucewichGameMode> Cur = GetClass();
	auto Arr = GetSaucewichInstance()->GetGameModes();
	Arr.RemoveSingleSwap(Cur);
	return Arr.Num() > 0 ? Arr[FMath::RandHelper(Arr.Num())] : Cur;
}

FString ASaucewichGameMode::ChooseNextMap(const UWorld* const World) const
{
	const auto Cur = World->GetName();
	TArray<FString> Maps;
	for (auto&& Map : Data.Maps)
	{
		auto Name = Map.GetAssetName();
		if (Name != Cur) Maps.Add(MoveTemp(Name));
	}
	return Maps.Num() > 0 ? Maps[FMath::RandHelper(Maps.Num())] : Cur;
}

void ASaucewichGameMode::OnPlayerChangedName(ASaucewichPlayerState* const Player, FString&& OldName)
{
	PrintMessage(FMT_MSG(LOCTEXT("NameChange", "{0}님이 이름을 {1}|hpp(으로,로) 변경했습니다."),
		FText::FromString(MoveTemp(OldName)), FText::FromString(Player->GetPlayerName())), EMsgType::Left);
}

void ASaucewichGameMode::HandleMatchEnding()
{
	GetWorldTimerManager().SetTimer(MatchStateTimer, this, &AGameMode::EndMatch, Data.MatchEndingTime + .75f);
}

void ASaucewichGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (GameSession) GameSession->MaxPlayers = Data.MaxPlayers;
}

void ASaucewichGameMode::BeginPlay()
{
	Super::BeginPlay();
	auto&& TimerManager = GetWorldTimerManager();
	
	TimerManager.SetTimer(MatchStateUpdateTimer,
		this, &ASaucewichGameMode::UpdateMatchState,
		Data.MatchStateUpdateInterval, true
	);

	TimerManager.SetTimer(CheckIfNoPlayersTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]{EndMatchIfNoPlayers();}),
		30.f, true
	);

	check(TeamStarts.Num() == 0);
	TeamStarts.AddDefaulted(Data.Teams.Num());
	for (const auto Start : TActorRange<APlayerStart>{GetWorld()})
	{
		const auto Tag = Start->PlayerStartTag.ToString();
		const auto Team = FCString::Atoi(*Tag);
		if (!Tag.IsEmpty() && FChar::IsDigit(Tag[0]) && TeamStarts.IsValidIndex(Team))
		{
			TeamStarts[Team].Add(Start);
		}
	}

	USaucewichInstance::Get(this)->OnGameReady();
}

void ASaucewichGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	
#if WITH_GAMELIFT
	if (!ErrorMessage.IsEmpty()) return;

	static const FString SessionIDKey = TEXT("SessionID");
	const auto SessionID = UGameplayStatics::ParseOption(Options, SessionIDKey);
	if (SessionID.IsEmpty())
	{
		ErrorMessage = TEXT("SessionID not provided.");
		return;
	}
	
	const auto Result = GameLift::Get().AcceptPlayerSession(SessionID);
	if (!Result.IsSuccess())
	{
		ErrorMessage = Result.GetError().m_errorMessage;
		if (ErrorMessage.IsEmpty()) ErrorMessage = TEXT("Unable to accept player session.");
		return;
	}
#endif
}

FString ASaucewichGameMode::InitNewPlayer(APlayerController* const NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	check(NewPlayerController);
	
	const auto PC = Cast<ASaucewichPlayerController>(NewPlayerController);
	if (!PC) return {};

	if (!PC->PlayerState) return TEXT("PlayerState is null");

	GameSession->RegisterPlayer(PC, UniqueId.GetUniqueNetId(), false);
	
	auto PlayerName = UGameplayStatics::ParseOption(Options, SSTR("PlayerName"));
	
	if (USaucewich::IsValidPlayerName(PlayerName) != ENameValidity::Valid)
		PlayerName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), PC->PlayerState->PlayerId);
	
	ChangeName(PC, PlayerName, false);

#if WITH_GAMELIFT
	PC->SetSessionID(UGameplayStatics::ParseOption(Options, SSTR("SessionID")));
	PC->SetPlayerID(UGameplayStatics::ParseOption(Options, SSTR("PlayerID")));
	USaucewichInstance::Get(this)->IDtoPC.Add(PC->GetPlayerID(), PC);
#endif
	
	return {};
}

void ASaucewichGameMode::PostLogin(APlayerController* const NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	PrintMessage(
		FMT_MSG(LOCTEXT("Login", "{0}님이 게임에 참여했습니다."), FText::FromString(NewPlayer->PlayerState->GetPlayerName())),
		EMsgType::Left
	);
}

#if WITH_GAMELIFT
namespace GameLift
{
	extern void Check(const FGameLiftGenericOutcome& Outcome);
}

void ASaucewichGameMode::OnProcessTerminate()
{
	bTerminating = true;
	
	const auto GS = CastChecked<ASaucewichGameState>(GameState);
	const auto CurRemaining = GS->GetRemainingRoundSeconds();
	const auto Time = GameLift::Get().GetTerminationTime().GetResult() - (Data.MatchEndingTime + Data.NextGameWaitTime);
	
	if (0 < Time && Time < CurRemaining)
	{
		GS->SetRemainingRoundSeconds(Time);
	}
	else if (!HasMatchStarted())
	{
		GameLift::SafeTerminate();
	}
}
#endif

void ASaucewichGameMode::Logout(AController* const Exiting)
{
	Super::Logout(Exiting);
	
	PrintMessage(FMT_MSG(LOCTEXT("Logout", "{0}님이 게임에서 나갔습니다."),
		FText::FromString(Exiting->PlayerState->GetPlayerName())), EMsgType::Left);
	
#if WITH_GAMELIFT
	auto&& GameLiftSDK = GameLift::Get();
	if (const auto PC = Cast<ASaucewichPlayerController>(Exiting))
	{
		GameLiftSDK.RemovePlayerSession(PC->GetSessionID());
		USaucewichInstance::Get(this)->IDtoPC.Remove(PC->GetPlayerID());
	}
#endif

	if (HasMatchStarted())
		if (EndMatchIfNoPlayers())
			return;
}

void ASaucewichGameMode::HandleStartingNewPlayer_Implementation(APlayerController* const NewPlayer)
{
}

void ASaucewichGameMode::GenericPlayerInitialization(AController* const C)
{
	Super::GenericPlayerInitialization(C);
	
	const auto GS = CastChecked<ASaucewichGameState>(GameState);
	const auto PS = CastChecked<ASaucewichPlayerState>(C->PlayerState);
	
	const auto MinTeamNum = GS->GetNumPlayers(GS->GetMinPlayerTeam());
	const auto MyTeamNum = GS->GetNumPlayers(PS->GetTeam());
	
	if (!PS->IsValidTeam() || MyTeamNum - MinTeamNum > 1)
	{
		PS->SetTeam(GS->GetMinPlayerTeam());
	}
}

bool ASaucewichGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

AActor* ASaucewichGameMode::ChoosePlayerStart_Implementation(AController* const Player)
{
	const auto World = GetWorld();
	
#if WITH_EDITOR
	for (const auto Start : TActorRange<APlayerStartPIE>{World}) return Start;
#endif
	
	const auto PawnClass = GetDefaultPawnClassForController(Player);
	const auto PawnToFit = PawnClass ? GetDefault<APawn>(PawnClass) : nullptr;
	const auto Team = CastChecked<ASaucewichPlayerState>(Player->PlayerState)->GetTeam();

	TArray<APlayerStart*> StartsPriority[3];

	// TODO: 원래 팀이 invalid할 수가 없는데 새 게임으로 넘어갈 때 발생하는 듯. 근본적 해결 필요.
	if (!TeamStarts.IsValidIndex(Team)) return nullptr;

	for (const auto Start : TeamStarts[Team])
	{
		auto ActorLocation = Start->GetActorLocation();
		const auto ActorRotation = Start->GetActorRotation();
		if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
		{
			StartsPriority[0].Add(Start);
		}
		else if (StartsPriority[0].Num() == 0)
		{
			if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				StartsPriority[1].Add(Start);
			}
			else if (StartsPriority[1].Num() == 0)
			{
				StartsPriority[2].Add(Start);
			}
		}
	}

	for (auto&& Starts : StartsPriority)
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
	if (!NewPlayer->GetPawn())
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

		NewPlayer->GetPawn()->SetActorLocationAndRotation(
			StartSpot->GetActorLocation(),
			FRotator{0.f, StartSpot->GetActorRotation().Yaw, 0.f}
		);

		FinishRestartPlayer(NewPlayer, StartSpot->GetActorRotation());
	}
}

void ASaucewichGameMode::SetPlayerDefaults(APawn* const PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	PlayerPawn->GetPlayerStateChecked<ASaucewichPlayerState>()->GiveWeapons();
}

bool ASaucewichGameMode::ReadyToStartMatch_Implementation()
{
	return NumPlayers >= Data.MinPlayerToStart;
}

bool ASaucewichGameMode::ReadyToEndMatch_Implementation()
{
	if (CastChecked<ASaucewichGameState>(GameState)->GetRemainingRoundSeconds() <= 0.f)
	{
		return true;
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
}

void ASaucewichGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

#if WITH_GAMELIFT
	if (bTerminating)
	{
		GameLift::SafeTerminate();
	}
	else
#endif
	{
		GetWorldTimerManager().SetTimer(MatchStateTimer, this, &ASaucewichGameMode::StartNextGame, Data.NextGameWaitTime);
	}
}

void ASaucewichGameMode::EndMatch()
{
	if (GetMatchState() == MatchState::Ending)
		SetMatchState(MatchState::WaitingPostMatch);
}

bool ASaucewichGameMode::EndMatchIfNoPlayers()
{
	if (NumPlayers == 0 && IsNetMode(NM_DedicatedServer))
	{
#if WITH_GAMELIFT
		if (bTerminating)
		{
			GameLift::SafeTerminate();
		}
		else
		{
			UE_LOG(LogGameLift, Log, TEXT("There's no one left. Terminating the game session..."))
			GameLift::Check(GameLift::Get().TerminateGameSession());
#else
		{
#endif
			GetWorld()->ServerTravel(SSTR("DSDef"), true);
		}
		GetWorldTimerManager().ClearTimer(CheckIfNoPlayersTimer);
		return true;
	}
	return false;
}

void ASaucewichGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	if (GetMatchState() == MatchState::Ending)
	{
		HandleMatchEnding();
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
				StartMatch();
			}
			else if (!bAboutToStartMatch && !bTimerExists)
			{
				bAboutToStartMatch = true;
				GetWorldTimerManager().SetTimer(MatchStateTimer, Data.MatchStartingTime, false);
				PrintMessage(LOCTEXT("StartingMatch", "게임이 시작됩니다!"), EMsgType::Center, Data.MatchStartingTime);
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
			SetMatchState(MatchState::Ending);
		}
	}
}

USaucewichInstance* ASaucewichGameMode::GetSaucewichInstance() const
{
	return CastChecked<USaucewichInstance>(GetGameInstance());
}

void ASaucewichGameMode::StartNextGame() const
{
#if WITH_GAMELIFT
	if (bTerminating)
	{
		GameLift::SafeTerminate();
		return;
	}
#endif

	const auto GmClass = ChooseNextGameMode();
	const auto DefGm = GetDefault<ASaucewichGameMode>(GmClass.LoadSynchronous());
	auto URL = DefGm->ChooseNextMap(GetWorld());
	URL += TEXT("?game=");
	URL += *GmClass.ToString();
	GetWorld()->ServerTravel(URL, true);
}

#undef LOCTEXT_NAMESPACE
