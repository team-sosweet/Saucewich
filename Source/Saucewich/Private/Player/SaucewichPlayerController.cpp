// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "SaucewichGameMode.h"

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
	if (GetRemainingRespawnTime() <= 0.f) ServerRespawn();
}

void ASaucewichPlayerController::ServerRespawn_Implementation()
{
	// 왠진 모르겠지만 RestartPlayer를 바로 호출할 경우 제대로 작동하지 않는다.
	// 게다가 항상 오작동하는 것도 아니다. 복불복이다. 상당히 화가 난다.
	// 그래서 다음 틱에서 호출하도록 했다. 찜찜하지만 작동은 잘 된다.
	GetWorldTimerManager().SetTimerForNextTick([this]
	{
		if (GetRemainingRespawnTime() > 0.f) return;
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
