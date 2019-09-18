// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Online/SaucewichGameMode.h"
#include "MakeSandwich.generated.h"

UCLASS()
class SAUCEWICH_API AMakeSandwich final : public ASaucewichGameMode
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	bool ReadyToEndMatch_Implementation() override;
	
private:
	void SpawnPerk() const;
	
	TArray<class APickupSpawnVolume*> PerkSpawnVolumes;
	FTimerHandle PerkSpawnTimer;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PerkClasses;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0))
	float PerkSpawnInterval;
};
