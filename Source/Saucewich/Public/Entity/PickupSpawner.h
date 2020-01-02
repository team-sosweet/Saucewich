// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

UCLASS()
class SAUCEWICH_API APickupSpawner : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Body;

public:
	APickupSpawner();
	
	void PickedUp();
	void SetSpawnTimer();
	auto GetSpawnClass() const { return Class; }

	UFUNCTION(BlueprintCallable)
	float GetSpawnInterval() const;
	float GetSpawnInterval(const class AGameStateBase* GS) const;

	UFUNCTION(BlueprintCallable)
	float GetRemainingSpawnTime() const;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Spawn();

	FTimerHandle SpawnTimer;

	// 스폰할 픽업의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<class APickup> Class;

	// 스폰 간격 오버라이드 (초)
	UPROPERTY(EditAnywhere, meta=(UIMin=0))
	float SpawnIntervalOverride;

	UPROPERTY(Replicated, Transient)
	float TimerStartTime = -1;
};
