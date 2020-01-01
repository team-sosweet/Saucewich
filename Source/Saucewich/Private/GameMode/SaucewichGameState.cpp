// Copyright 2019 Othereum. All Rights Reserved.

#include "GameMode/SaucewichGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacter.h"
#include "GameMode/SaucewichGameMode.h"

template <class Fn>
void ForEachEveryPlayer(const TArray<APlayerState*>& PlayerArray, Fn&& Do)
{
	for (const auto PS : PlayerArray)
		Do(CastChecked<ASaucewichPlayerState>(PS));
}

template <class Fn>
void ForEachPlayer(const TArray<APlayerState*>& PlayerArray, const uint8 Team, Fn&& Do)
{
	ForEachEveryPlayer(PlayerArray, [Team, &Do](const auto P){if (P->GetTeam() == Team) Do(P);});
}

TArray<ASaucewichPlayerState*> ASaucewichGameState::GetPlayersByTeam(const uint8 Team) const
{
	TArray<ASaucewichPlayerState*> Players;
	ForEachPlayer(PlayerArray, Team, [&Players](const auto P){Players.Add(P);});
	return Players;
}

TArray<ATpsCharacter*> ASaucewichGameState::GetCharactersByTeam(const uint8 Team) const
{
	TArray<ATpsCharacter*> Characters;
	ForEachPlayer(PlayerArray, Team, [&](ASaucewichPlayerState* const P) {
		if (const auto C = P->GetPawn<ATpsCharacter>()) Characters.Add(C);
	});
	return Characters;
}

ASaucewichGameState::ASaucewichGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

uint8 ASaucewichGameState::GetMinPlayerTeam() const
{
	TArray<uint8> Num;
	Num.AddZeroed(GetGmData().Teams.Num());
	ForEachEveryPlayer(PlayerArray, [&](ASaucewichPlayerState* P)
	{
		if (P->IsValidTeam()) ++Num[P->GetTeam()];
	});
	
	TArray<uint8> Min{0};
	for (auto i = 1; i < Num.Num(); ++i)
	{
		const auto This = Num[i];
		const auto MinV = Num[Min[0]];
		if (This < MinV)
		{
			Min.Reset();
			Min.Add(i);
		}
		else if (This == MinV)
		{
			Min.Add(i);
		}
	}
	return Min[FMath::RandHelper(Min.Num())];
}

void ASaucewichGameState::SetTeamScore(const uint8 Team, const int32 NewScore)
{
	if (TeamScore[Team] == NewScore || !IsMatchInProgress()) return;

	const auto GameMode = CastChecked<ASaucewichGameMode>(GetWorld()->GetAuthGameMode());
	auto&& TeamName = GameMode->GetData().Teams[Team].Name;
	
	const auto Msg = FText::FormatNamed(TeamScoreAddMsgFmt,
		TEXT("Team"), TeamName,
		TEXT("Score"), NewScore - TeamScore[Team]
	);

	GameMode->PrintMessage(Msg, EMsgType::Center);
	
	TeamScore[Team] = NewScore;
}

bool ASaucewichGameState::CanAddPersonalScore() const
{
	return IsMatchInProgress();
}

float ASaucewichGameState::GetRemainingRoundSeconds() const
{
	return IsMatchInProgress() ? FMath::Max(0.f, GetGmData().RoundMinutes * 60 - (GetServerWorldTimeSeconds() - RoundStartTime)) : 0;
}

void ASaucewichGameState::BeginPlay()
{
	Super::BeginPlay();
	TeamScore.AddZeroed(GetGmData().Teams.Num());
	TeamScoreAddMsgFmt = GetGmData().TeamScoreAddMsg;
}

void ASaucewichGameState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Dilation > KINDA_SMALL_NUMBER)
	{
		const auto Duration = GetGmData().MatchEndingTime;
		Dilation = FMath::Max(Dilation - DeltaTime / Duration, KINDA_SMALL_NUMBER);
		for (const auto Actor : DilatableActors) if (IsValid(Actor)) Actor->CustomTimeDilation = Dilation;
		for (const auto PSC : DilatablePSCs) if (IsValid(PSC)) PSC->CustomTimeDilation = Dilation;
	}
}

void ASaucewichGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (HasAuthority())
	{
		RoundStartTime = GetServerWorldTimeSeconds();
	}

	OnCleanup.Broadcast();

	auto LevelName = GetWorld()->GetName();
	LevelName += '_';
	LevelName += GetGmData().StreamLevelSuffix;
	UGameplayStatics::LoadStreamLevel(this, *LevelName, true, false, {});
}

void ASaucewichGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	if (HasAuthority())
	{
		WonTeam = GetWinningTeam();
		OnRep_WonTeam();
		
		if (WonTeam != uint8(-1))
		{
			ForEachPlayer(PlayerArray, WonTeam, [](ASaucewichPlayerState* const Player)
			{
				Player->AddScore(TEXT("Win"), 0, true);
			});

			UE_LOG(LogGameState, Log, TEXT("Match result: The [%d] %s team won the game!"),
			       static_cast<int>(WonTeam), *GetGmData().Teams[WonTeam].Name.ToString());
		}
		else
		{
			UE_LOG(LogGameState, Log, TEXT("Match result: Draw!"));
		}
	}
}

void ASaucewichGameState::HandleLeavingMap()
{
	Super::HandleLeavingMap();
	OnLeavingMap.Broadcast();
}

void ASaucewichGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichGameState, RoundStartTime);
	DOREPLIFETIME(ASaucewichGameState, TeamScore);
	DOREPLIFETIME(ASaucewichGameState, WonTeam);
}

void ASaucewichGameState::HandleMatchEnding()
{
	Dilation = 1.f;
}

void ASaucewichGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	if (GetMatchState() == MatchState::Ending)
	{
		HandleMatchEnding();
	}

	OnMatchStateChanged.Broadcast(GetMatchState());
}

void ASaucewichGameState::MulticastPlayerDeath_Implementation(
	ASaucewichPlayerState* const Victim, ASaucewichPlayerState* const Attacker, AActor* const Inflictor)
{
	if (Attacker) Attacker->OnKill();
	if (Victim) Victim->OnDeath();
	OnPlayerDeath.Broadcast(Victim, Attacker, Inflictor);
}

uint8 ASaucewichGameState::GetNumPlayers(const uint8 Team) const
{
	uint8 Num = 0;
	ForEachPlayer(PlayerArray, Team, [&Num](auto){++Num;});
	return Num;
}

uint8 ASaucewichGameState::GetWinningTeam() const
{
	constexpr auto Invalid = static_cast<uint8>(-1);
	if (WonTeam != uint8(-2))
	{
		UE_LOG(LogGameState, Log, TEXT("GetWinningTeam() returned %d because of WonTeam != Invalid"), WonTeam);
		return WonTeam;
	}

	const auto Empty = GetEmptyTeam();
	if (Empty != Invalid)
	{
		UE_LOG(LogGameState, Log, TEXT("GetWinningTeam() returned %d because of Empty != Invalid"), 1 - Empty);
		return 1 - Empty;
	}
	
	const auto A = GetTeamScore(0), B = GetTeamScore(1);
	const auto Ret = A > B ? 0 : A < B ? 1 : -1;
	UE_LOG(LogGameState, Log, TEXT("GetWinningTeam() returned %d. Ketchup: %d, Mustard: %d"), Ret, A, B);
	return Ret;
}

uint8 ASaucewichGameState::GetEmptyTeam() const
{
	auto T0 = 0, T1 = 0;
	ForEachEveryPlayer(PlayerArray, [&T0, &T1](ASaucewichPlayerState* const Ply)
	{
		const auto T = Ply->GetTeam();
		if (T == 0) ++T0;
		else if (T == 1) ++T1;
	});
	if (T0 == 0 && T1 > 0) return 0;
	if (T0 > 0 && T1 == 0) return 1;

	return -1;
}

const FGameData& ASaucewichGameState::GetGmData() const
{
	return CastChecked<ASaucewichGameMode>(GetDefaultGameMode())->GetData();
}

void ASaucewichGameState::OnRep_WonTeam() const
{
	OnMatchEnd.Broadcast(WonTeam);
}
