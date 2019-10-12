// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichPlayerController.h"

#include "CoreDelegates.h"
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
	if (!Delegate.IsBound()) return;
	const auto PS = GetPlayerState<ASaucewichPlayerState>();
	if (IsValid(PS)) Delegate.Execute(PS);
	else OnPlayerStateSpawned.AddUnique(Delegate);
}

void ASaucewichPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &ThisClass::Ping);
		FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &ThisClass::Ping);
	}
}

bool ASaucewichPlayerController::CanRespawn() const
{
	const auto Char = Cast<ATpsCharacter>(GetPawn());
	return !Char || !Char->IsAlive() && GetRemainingRespawnTime() <= 0.f;
}

void ASaucewichPlayerController::Ping()
{
	GetWorldTimerManager().SetTimer(PingTimer, this, &ASaucewichPlayerController::OnPingFailed, PingTimeout);
	ServerPing();
}

void ASaucewichPlayerController::OnPingFailed_Implementation()
{
	const auto World = GetWorld();
	const auto NetDriver = World->GetNetDriver();
	GEngine->BroadcastNetworkFailure(World, NetDriver, ENetworkFailure::ConnectionTimeout);
	if (NetDriver->ServerConnection) NetDriver->ServerConnection->Close();
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
	if (!Delegate.IsBound()) return;
	if (const auto Char = Cast<ATpsCharacter>(GetCharacter()))
	{
		Delegate.Execute(Char);
	}
	else
	{
		OnCharacterSpawned.AddUnique(Delegate);
	}
}

void ASaucewichPlayerController::PrintMessage_Implementation(const FName MessageID, const float Duration, const EMsgType Type)
{
	const auto GS = GetWorld()->GetGameState();
	if (!GS) return;

	const auto DefGm = GS->GetDefaultGameMode<ASaucewichGameMode>();
	if (!DefGm) return;

	if (const auto Found = DefGm->GetMessage(MessageID))
	{
		OnReceiveMessage.Broadcast(*Found, Duration, Type);
	}
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
