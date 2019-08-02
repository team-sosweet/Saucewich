// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "SaucewichGameMode.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "Weapon.h"
#include "WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichPlayerState, Log, All)

void ASaucewichPlayerState::SetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	MulticastSetWeapon(Slot, Weapon);
}

bool ASaucewichPlayerState::SetWeapon_Validate(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	return Weapon && Slot < Weapons.Num();
}

void ASaucewichPlayerState::MulticastSetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	Weapons[Slot] = Weapon;
	SafeGameState([this, Slot, Weapon]
	{
		GameState->OnPlayerChangedWeapon.Broadcast(this, Slot, Weapon);
	});
}

void ASaucewichPlayerState::GiveWeapons()
{
	if (const auto Character = GetPawn<ATpsCharacter>())
	{
		for (auto i = 0; i < Weapons.Num(); ++i)
		{
			if (const auto Weapon = Weapons[i] ? Weapons[i] : i < DefaultWeapons.Num() ? DefaultWeapons[i] : nullptr)
			{
				Character->GetWeaponComponent()->Give(Weapon);
			}
		}
	}
}

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	const auto OldTeam = Team;
	Team = NewTeam;
	OnTeamChanged(OldTeam);
}

void ASaucewichPlayerState::OnTeamChanged(const uint8 OldTeam)
{
	OnTeamChangedDelegate.Broadcast(Team);
	SafeGameState([this, OldTeam]
	{
		GameState->OnPlayerChangedTeam.Broadcast(this, OldTeam, Team);
	});
}

void ASaucewichPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Init();
	Weapons = DefaultWeapons;
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
}

void ASaucewichPlayerState::Init()
{
	if (const auto GS = GetWorld()->GetGameState())
	{
		if (const auto SaucewichGS = Cast<ASaucewichGameState>(GS))
		{
			GameState = SaucewichGS;
			OnGameStateReady.Broadcast();
			OnGameStateReady.Clear();
		}
		else
		{
			UE_LOG(LogSaucewichPlayerState, Error, TEXT("이런! 게임 스테이트가 있긴 한데 ASaucewichGameState가 아니네요!"));
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ASaucewichPlayerState::Init);
	}
}
