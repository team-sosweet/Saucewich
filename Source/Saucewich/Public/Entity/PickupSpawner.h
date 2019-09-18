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

	auto GetSpawnClass() const { return Class; }

	UFUNCTION(BlueprintCallable)
	float GetSpawnInterval() const;

	UFUNCTION(BlueprintCallable)
	float GetRemainingSpawnTime() const;

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Spawn();

	UFUNCTION()
	void SetSpawnTimer();

	FTimerHandle SpawnTimer;

	// 스폰할 픽업의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<class APickup> Class;

	// 스폰 간격 오버라이드 (초)
	UPROPERTY(EditAnywhere, meta=(UIMin=0))
	float SpawnIntervalOverride;

	UPROPERTY(ReplicatedUsing=SetSpawnTimer, Transient)
	uint8 bSpawnTimerFlag : 1;
};
