// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Player/SaucewichPlayerState.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Player/SaucewichPlayerController.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponComponent.h"
#include "Saucewich.h"
#include "SaucewichInstance.h"
#include "Names.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Log, All)

void ASaucewichPlayerState::SetWeapon(const uint8 Slot, const TSoftClassPtr<class AWeapon>& Weapon)
{
	SetWeapon_Internal(Slot, Weapon);
	if (!HasAuthority()) ServerSetWeapon(Slot, Weapon);
	if (CastChecked<AController>(GetOwner())->IsLocalController()) SaveWeaponLoadout();
}

void ASaucewichPlayerState::SaveWeaponLoadout()
{
	SaveConfig();
}

void ASaucewichPlayerState::GiveWeapons()
{
	const auto Character = CastChecked<ATpsCharacter>(GetPawn());

	for (const auto Weapon : WeaponLoadout)
		Character->GetWeaponComponent()->Give(Weapon);
}

void ASaucewichPlayerState::OnKill()
{
	if (!HasAuthority()) return;

	const auto GS = CastChecked<AGameState>(GetWorld()->GetGameState());
	if (!GS->IsMatchInProgress()) return;

	++Kill;
	AddScore(NAME("Kill"));
}

void ASaucewichPlayerState::OnDeath()
{
	if (!HasAuthority()) return;

	const auto GS = GetWorld()->GetGameState<AGameState>();
	if (!GS || !GS->IsMatchInProgress()) return;

	++Death;
}

void ASaucewichPlayerState::AddScore(const FName ScoreID, int32 ActualScore, const bool bForce)
{
	if (!HasAuthority()) return;

	if (!bForce)
	{
		const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
		if (!GS->CanAddPersonalScore()) return;
	}

	const auto GI = GetWorld()->GetGameInstanceChecked<USaucewichInstance>();
	if (ActualScore == 0)
		ActualScore = GI->GetScoreData(ScoreID).Score;

	float NewScore = GetScore() + ActualScore;
	SetScore(NewScore);
	MulticastAddScore(ScoreID, ActualScore, static_cast<int32>(NewScore));

	UE_LOG(LogPlayerState, Log, TEXT("Add %d score to %s by %s"), ActualScore, *GetPlayerName(), *ScoreID.ToString())
}

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	if (Team != NewTeam)
	{
		const auto OldTeam = Team;
		Team = NewTeam;
		OnTeamChanged(OldTeam);
	}
}

void ASaucewichPlayerState::OnTeamChanged(const uint8 OldTeam)
{
	OnTeamChangedDelegate.Broadcast(Team);
	OnTeamChangedNative.Broadcast(Team);

	if (const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState(), ECastCheckedType::NullAllowed))
		GS->OnPlayerChangedTeam.Broadcast(this, OldTeam, Team);
}

void ASaucewichPlayerState::SetWeapon_Internal(const uint8 Slot, const TSoftClassPtr<AWeapon>& Weapon)
{
	WeaponLoadout[Slot] = Weapon;
}

void ASaucewichPlayerState::ValidateLoadout()
{
	const auto OrigNum = WeaponLoadout.Num();
	const auto DefNum = DefaultWeaponLoadout.Num();
	
	WeaponLoadout.SetNum(DefNum);

	const auto Validate = [](TArray<TSoftClassPtr<AWeapon>>& Arr, const int32 Num, auto&& Fn)
	{
		for (auto i=0; i<Num; ++i)
		{
			const auto Cls = Arr[i].LoadSynchronous();
			if (!Cls || GetDefault<AWeapon>(Cls)->GetData().Slot != i)
			{
				Fn(i);
			}
		}
	};

#if DO_CHECK
	if (DefNum == 0)
	{
		UE_LOG(LogPlayerState, Error, TEXT("Default weapon loadout not set for %s"), *GetClass()->GetName());
	}
	else
	{
		Validate(DefaultWeaponLoadout, DefNum, [this](const int32 Idx)
		{
			UE_LOG(LogPlayerState, Error, TEXT("Invalid slot of default weapon (index %d) for %s"), Idx, *GetClass()->GetName());
		});
	}
#endif

	Validate(WeaponLoadout, FMath::Min(OrigNum, DefNum), [&](const int32 Idx)
	{
		WeaponLoadout[Idx] = DefaultWeaponLoadout[Idx];
	});
	
	for (auto i=OrigNum; i<DefNum; ++i)
		WeaponLoadout[i] = DefaultWeaponLoadout[i];
}

void ASaucewichPlayerState::ServerSetWeaponLoadout_Implementation(const TArray<TSoftClassPtr<AWeapon>>& Loadout)
{
	WeaponLoadout = Loadout;
}

bool ASaucewichPlayerState::ServerSetWeaponLoadout_Validate(const TArray<TSoftClassPtr<AWeapon>>& Loadout)
{
	return true;
}

void ASaucewichPlayerState::MulticastAddScore_Implementation(const FName ScoreName, const int32 ActualScore, const int32 NewScore)
{
	OnScoreAddedNative.Broadcast(ScoreName, ActualScore, NewScore);
	OnScoreAdded.Broadcast(ScoreName, ActualScore, NewScore);
}

void ASaucewichPlayerState::ServerSetWeapon_Implementation(const uint8 Slot, const TSoftClassPtr<AWeapon>& Weapon)
{
	SetWeapon_Internal(Slot, Weapon);
}

bool ASaucewichPlayerState::ServerSetWeapon_Validate(const uint8 Slot, const TSoftClassPtr<AWeapon>& Weapon)
{
	return true;
}

void ASaucewichPlayerState::SetObjective(const uint8 NewObjective)
{
	if (!HasAuthority()) return;

	if (Objective == NewObjective) return;

	const auto GS = GetWorld()->GetGameState<AGameState>();
	if (!GS || !GS->IsMatchInProgress()) return;

	Objective = NewObjective;
}

void ASaucewichPlayerState::BindOnTeamChanged(FOnTeamChangedNative::FDelegate&& Callback)
{
	Callback.Execute(Team);
	OnTeamChangedNative.Add(MoveTemp(Callback));
}

void ASaucewichPlayerState::RequestSetPlayerName_Implementation(const FString& NewPlayerName)
{
	if (GetPlayerName() != NewPlayerName && USaucewich::IsValidPlayerName(NewPlayerName) == ENameValidity::Valid)
	{
		SetPlayerName(NewPlayerName);
	}
}

bool ASaucewichPlayerState::RequestSetPlayerName_Validate(const FString& NewPlayerName)
{
	return true;
}

void ASaucewichPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ValidateLoadout();
	
	if (const auto PC = Cast<ASaucewichPlayerController>(GetOwner()))
	{
		ASaucewichPlayerController::BroadcastPlayerStateSpawned(PC, this);
	}
	
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerSetWeaponLoadout(WeaponLoadout);
	}
}

void ASaucewichPlayerState::SetPlayerName(const FString& S)
{
	for (const auto Player : TActorRange<APlayerState>{GetWorld()})
	{
		if (Player == this || !IsValid(Player)) continue;
		
		auto Name = Player->GetPlayerName();
		if (Name.Equals(S, ESearchCase::IgnoreCase))
		{
			auto Cnt = 0;
			for (auto i = Name.Len() - 1; i >= 0 && isdigit(Name[i]); --i, ++Cnt) {}
			const auto Num = FCString::Atoi(*Name + Name.Len() - Cnt) + 1;
			Name.RemoveAt(Name.Len() - Cnt, Cnt, false);
			Name.AppendInt(Num);
			return SetPlayerName(Name);
		}
	}

	auto Old = GetPlayerName();
	Super::SetPlayerName(S);

	if (!Old.IsEmpty())
		if (const auto GameMode = GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
			GameMode->OnPlayerChangedName(this, MoveTemp(Old));
}

void ASaucewichPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	OnNameChanged.Broadcast(GetPlayerName());
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
	DOREPLIFETIME(ASaucewichPlayerState, Objective);
	DOREPLIFETIME(ASaucewichPlayerState, Kill);
	DOREPLIFETIME(ASaucewichPlayerState, Death);
}
