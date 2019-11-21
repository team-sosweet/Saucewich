// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameMode/SaucewichGameMode.h"
#include "MakeSandwich.generated.h"

UCLASS()
class SAUCEWICH_API AMakeSandwich : public ASaucewichGameMode
{
	GENERATED_BODY()

protected:
	void HandleMatchHasStarted() override;
	
private:
	void SpawnPerk() const;
	
	TArray<class APickupSpawnVolume*> PerkSpawnVolumes;
	FTimerHandle PerkSpawnTimer;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PerkClasses;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
	float PerkSpawnInterval;
};
