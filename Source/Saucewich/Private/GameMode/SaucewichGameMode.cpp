// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "SaucewichGameMode.h"

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

#define LOCTEXT_NAMESPACE ""

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

void ASaucewichGameMode::PrintMessage(const FText& Message, const EMsgType Type, const float Duration) const
{
	for (const auto PC : TActorRange<ASaucewichPlayerController>{GetWorld()})
	{
		PC->PrintMessage(Message, Duration, Type);
	}
	UE_LOG(LogGameMode, Log, TEXT("%s"), *Message.ToString());
}

template <class T, class Alloc>
static T RandomDistinct(TArray<T, Alloc> Arr, const T& Elem)
{
	Arr.RemoveSingleSwap(Elem, false);
	return Arr.Num() > 0 ? Arr[FMath::RandHelper(Arr.Num())] : Elem;
}

TSoftClassPtr<ASaucewichGameMode> ASaucewichGameMode::ChooseNextGameMode() const
{
	return RandomDistinct<TSoftClassPtr<ASaucewichGameMode>>(GetSaucewichInstance()->GetGameModes(), GetClass());
}

TSoftObjectPtr<UWorld> ASaucewichGameMode::ChooseNextMap() const
{
	return RandomDistinct<TSoftObjectPtr<UWorld>>(Data.Maps, GetWorld());
}

void ASaucewichGameMode::OnPlayerChangedName(ASaucewichPlayerState* const Player, FString&& OldName) const
{
	PrintMessage(
		FMT_MSG(
			LOCTEXT("NameChange", "{0}님이 이름을 {1}|hpp(으로,로) 변경했습니다."),
			FText::FromString(MoveTemp(OldName)), FText::FromString(Player->GetPlayerName())
		),
		EMsgType::Left
	);
}

void ASaucewichGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (GameSession) GameSession->MaxPlayers = Data.MaxPlayers;
}

void ASaucewichGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(
		MatchStateUpdateTimer, this, &ASaucewichGameMode::UpdateMatchState, Data.MatchStateUpdateInterval, true
	);

	TeamStarts.SetNumZeroed(Data.Teams.Num());
	for (const auto Start : TActorRange<APlayerStart>{GetWorld()})
	{
		TeamStarts[Start->PlayerStartTag.ToString()[0] - TEXT('0')].Add(Start);
	}
}

void ASaucewichGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!ErrorMessage.IsEmpty()) return;

#if WITH_GAMELIFT
	auto& GameLiftSdkModule = USaucewich::GetGameLift();
	const auto Result = GameLiftSdkModule.AcceptPlayerSession(UGameplayStatics::ParseOption(Options, TEXT("SessionID")));
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
	check(NewPlayerController);
	
	const auto PC = Cast<ASaucewichPlayerController>(NewPlayerController);
	if (!PC) return {};

	if (!PC->PlayerState)
		return TEXT("PlayerState is null");

	GameSession->RegisterPlayer(PC, UniqueId.GetUniqueNetId(), false);
	
	auto PlayerName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
	
	if (USaucewich::IsValidPlayerName(PlayerName) != ENameValidity::Valid)
		PlayerName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), PC->PlayerState->PlayerId);
	
	ChangeName(PC, PlayerName, false);

	PC->SetSessionID(UGameplayStatics::ParseOption(Options, TEXT("SessionID")));
	
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

void ASaucewichGameMode::Logout(AController* const Exiting)
{
	Super::Logout(Exiting);
	
	PrintMessage(
		FMT_MSG(LOCTEXT("Logout", "{0}님이 게임에서 나갔습니다."), FText::FromString(Exiting->PlayerState->GetPlayerName())),
		EMsgType::Left
	);

#if WITH_GAMELIFT
	if (const auto PC = Cast<ASaucewichPlayerController>(Exiting))
	{
		auto& Module = USaucewich::GetGameLift();
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
		auto&& Transform = StartSpot->GetRootComponent()->GetComponentTransform();
		NewPlayer->GetPawn()->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
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

	if (NumPlayers <= 0)
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
	
	if (NumPlayers == 0)
	{
		StartNextGame();
	}
	else
	{
		GetWorldTimerManager().SetTimer(MatchStateTimer, this, &ASaucewichGameMode::StartNextGame, Data.NextGameWaitTime);
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
			EndMatch();
		}
	}
}

USaucewichInstance* ASaucewichGameMode::GetSaucewichInstance() const
{
	return CastChecked<USaucewichInstance>(GetGameInstance());
}

void ASaucewichGameMode::StartNextGame() const
{
	const auto GmClass = ChooseNextGameMode();
	const auto DefGm = GetDefault<ASaucewichGameMode>(GmClass.LoadSynchronous());
	const auto NewMap = DefGm->ChooseNextMap();

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap.GetAssetName(), *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}
