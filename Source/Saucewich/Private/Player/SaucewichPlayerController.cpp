// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Player/SaucewichPlayerController.h"

#include "Misc/CoreDelegates.h"
#include "Engine/Engine.h"
#include "Engine/NetConnection.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Player/SaucewichPlayerState.h"
#include "Player/BaseHUD.h"
#include "Widget/ErrorWidget.h"

#define LOCTEXT_NAMESPACE ""

void ASaucewichPlayerController::SetRespawnTimer_Implementation(const float RespawnTime)
{
	GetWorldTimerManager().SetTimer(RespawnTimer, RespawnTime, false);
}

float ASaucewichPlayerController::GetRemainingRespawnTime() const
{
	return FMath::Max(GetWorldTimerManager().GetTimerRemaining(RespawnTimer), 0.f);
}

void ASaucewichPlayerController::Respawn()
{
	if (CanRespawn()) ServerRespawn();
}

void ASaucewichPlayerController::SafePlayerState(const FOnPlayerStateSpawnedSingle& Delegate)
{
	check(Delegate.IsBound());
	if (IsValid(PlayerState)) Delegate.Execute(CastChecked<ASaucewichPlayerState>(PlayerState));
	else OnPlayerStateSpawned.AddUnique(Delegate);
}

void ASaucewichPlayerController::SafePS(FOnPSSpawnedNative::FDelegate&& Delegate)
{
	if (IsValid(PlayerState)) Delegate.Execute(CastChecked<ASaucewichPlayerState>(PlayerState));
	else OnPSSpawnedNative.Add(MoveTemp(Delegate));
}

void ASaucewichPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(LatencyMeasureTimer, this, &ASaucewichPlayerController::MeasureLatency, 10.f, true);
	}
	else
	{
		FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &ThisClass::Ping);
		FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &ThisClass::Ping);
		InitialMeasureLatency();
	}
}

void ASaucewichPlayerController::InitPlayerState()
{
	if (GetNetMode() != NM_Client)
	{
		const auto World = GetWorld();
		const AGameModeBase* GameMode = World ? World->GetAuthGameMode() : nullptr;

		if (!GameMode)
		{
			const AGameStateBase* const GameState = World ? World->GetGameState() : nullptr;
			GameMode = GameState ? GameState->GetDefaultGameMode() : nullptr;
		}

		if (GameMode)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;

			auto PlayerStateClassToSpawn = GameMode->PlayerStateClass;
			if (!PlayerStateClassToSpawn) PlayerStateClassToSpawn = ASaucewichPlayerState::StaticClass();

			PlayerState = World->SpawnActor<APlayerState>(PlayerStateClassToSpawn, SpawnInfo);
		}
	}
}

bool ASaucewichPlayerController::CanRespawn() const
{
	const auto Char = CastChecked<ATpsCharacter>(GetPawn(), ECastCheckedType::NullAllowed);
	return !Char || (!Char->IsAlive() && GetRemainingRespawnTime() <= 0.f);
}

void ASaucewichPlayerController::Ping()
{
	GetWorldTimerManager().SetTimer(PingTimer, this, &ASaucewichPlayerController::OnPingFailed, PingTimeout);
	ServerPing();
}

void ASaucewichPlayerController::OnPingFailed() const
{
	const auto ServerConnection = GetWorld()->GetNetDriver()->ServerConnection;
	if (ServerConnection) ServerConnection->Close();
}

void ASaucewichPlayerController::MeasureLatency()
{
	LatencyMeasureBeginTime = GetWorld()->GetRealTimeSeconds();
	BeginMeasureLatency();
}

void ASaucewichPlayerController::InitialMeasureLatency_Implementation()
{
	MeasureLatency();
}

bool ASaucewichPlayerController::InitialMeasureLatency_Validate()
{
	return true;
}

void ASaucewichPlayerController::BeginMeasureLatency_Implementation()
{
	ReplyMeasureLatency();
}

void ASaucewichPlayerController::ReplyMeasureLatency_Implementation()
{
	LatencyInMs = GetWorld()->GetRealTimeSeconds() - LatencyMeasureBeginTime;
}

bool ASaucewichPlayerController::ReplyMeasureLatency_Validate()
{
	return true;
}

void ASaucewichPlayerController::ServerPing_Implementation()
{
	ClientPing();
}

bool ASaucewichPlayerController::ServerPing_Validate()
{
	return true;
}

void ASaucewichPlayerController::ClientPing_Implementation()
{
	GetWorldTimerManager().ClearTimer(PingTimer);
}

void ASaucewichPlayerController::SafeCharacter(const FOnCharacterSpawnedSingle& Delegate)
{
	check(Delegate.IsBound());
	if (const auto Char = GetCharacter())
	{
		Delegate.Execute(CastChecked<ATpsCharacter>(Char));
	}
	else
	{
		OnCharacterSpawned.AddUnique(Delegate);
	}
}

void ASaucewichPlayerController::SetSessionID(FString&& ID)
{
	ensure(SessionID.IsEmpty());
	SessionID = MoveTemp(ID);
}

const FString& ASaucewichPlayerController::GetSessionID() const
{
	return SessionID;
}

void ASaucewichPlayerController::SetPlayerID(FString&& ID)
{
	PlayerID = MoveTemp(ID);
}

const FString& ASaucewichPlayerController::GetPlayerID() const
{
	return PlayerID;
}

void ASaucewichPlayerController::BroadcastRespawn() const
{
	OnPlyRespawnNative.Broadcast();
	OnCharRespawn.Broadcast();
}

void ASaucewichPlayerController::BroadcastDeath() const
{
	OnPlyDeathNative.Broadcast();
	OnCharDied.Broadcast();
}

void ASaucewichPlayerController::PrintMessage_Implementation(const FText& Message, const float Duration, const EMsgType Type)
{
	OnReceiveMessage.Broadcast(Message, Duration, Type);
}

void ASaucewichPlayerController::ServerRespawn_Implementation()
{
	// 왠진 모르겠지만 RestartPlayer를 바로 호출할 경우 제대로 작동하지 않는다.
	// 게다가 항상 오작동하는 것도 아니다. 복불복이다. 상당히 화가 난다.
	// 그래서 다음 틱에서 호출하도록 했다. 찜찜하지만 작동은 잘 된다.
	GetWorldTimerManager().SetTimerForNextTick([this]
	{
		if (!CanRespawn()) return;
		
		if (const auto Gm = GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
		{
			Gm->RestartPlayer(this);
		}
	});
}

bool ASaucewichPlayerController::ServerRespawn_Validate()
{
	return true;
}

#undef LOCTEXT_NAMESPACE
