// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SaucewichPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitEnemy);

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

	// 적에게 피해를 입혔을 때 발동됩니다. 주 목적은 히트마커 입니다.
	UPROPERTY(BlueprintAssignable)
	FOnHitEnemy OnHitEnemy;

private:
	FTimerHandle RespawnTimer;
};
