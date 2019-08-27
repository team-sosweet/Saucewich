// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SaucewichPlayerController.generated.h"

UCLASS()
class SAUCEWICH_API ASaucewichPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void SetRespawnTimer(float RespawnTime);

	UFUNCTION(BlueprintCallable)
	float GetRemainingRespawnTime() const;

	void Respawn();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRespawn();

private:
	FTimerHandle RespawnTimer;
};
