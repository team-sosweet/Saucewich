// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/GameModeDependentLevelActor.h"
#include "PickupSpawner.generated.h"

UCLASS()
class SAUCEWICH_API APickupSpawner final : public AGameModeDependentLevelActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Body;

public:
	APickupSpawner();
	void PickedUp();

protected:
	void BeginPlay() override;

private:
	void Spawn();
	void SetSpawnTimer();

	FTimerHandle SpawnTimer;

	// 스폰할 픽업의 클래스
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickup> Class;

	// 스폰 간격 (초)
	UPROPERTY(EditAnywhere)
	float SpawnInterval;
};
