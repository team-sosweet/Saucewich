// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/SaucewichGameState.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacter.h"
#include "SaucewichGameMode.h"

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
	if (!IsMatchInProgress()) return;
	
	if (TeamScore.Num() <= Team) TeamScore.AddZeroed(Team - TeamScore.Num() + 1);
	
	UE_LOG(LogGameState, Log, TEXT("Added %d score to the [%d] %s team. Total score: %d"),
		NewScore - TeamScore[Team], static_cast<int32>(Team), *GetGmData().Teams[Team].Name.ToString(), NewScore);
	
	TeamScore[Team] = NewScore;
}

bool ASaucewichGameState::CanAddPersonalScore() const
{
	return IsMatchInProgress();
}

bool ASaucewichGameState::ShouldPlayerTakeDamage(const ATpsCharacter* Victim, float DamageAmount,
	const FDamageEvent& DamageEvent, const AController* EventInstigator, const AActor* DamageCauser) const
{
	return !HasMatchEnded();
}

float ASaucewichGameState::GetRemainingRoundSeconds() const
{
	return IsMatchInProgress() ? FMath::Max(0.f, GetGmData().RoundMinutes * 60 - (GetServerWorldTimeSeconds() - RoundStartTime)) : 0;
}

void ASaucewichGameState::BeginPlay()
{
	Super::BeginPlay();
	TeamScore.AddZeroed(GetGmData().Teams.Num());
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
		if (WonTeam != 0)
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

	OnCleanup.Broadcast();
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
	if (WonTeam != static_cast<uint8>(-1)) return WonTeam;

	const auto Empty = GetEmptyTeam();
	if (Empty) return 3 - Empty;
	
	const auto A = GetTeamScore(1), B = GetTeamScore(2);
	return A > B ? 1 : A < B ? 2 : 0;
}

uint8 ASaucewichGameState::GetEmptyTeam() const
{
	auto T1 = 0, T2 = 0;
	ForEachEveryPlayer(PlayerArray, [&T1, &T2](ASaucewichPlayerState* const Ply)
	{
		const auto T = Ply->GetTeam();
		if (T == 1) ++T1;
		else if (T == 2) ++T2;
	});
	if (T1 == 0 && T2 > 0) return 1;
	if (T1 > 0 && T2 == 0) return 2;

	return 0;
}

const FGameData& ASaucewichGameState::GetGmData() const
{
	return CastChecked<ASaucewichGameMode>(GetDefaultGameMode())->GetData();
}

void ASaucewichGameState::OnRep_WonTeam()
{
	OnMatchEnd.Broadcast(WonTeam);
}
