// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "Perk.generated.h"

UCLASS()
class SAUCEWICH_API APerk : public APickup
{
	GENERATED_BODY()

public:
	float GetDuration() const { return Duration; }
	auto GetParticle() const { return Particle; }
	
private:
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Particle;
	
	UPROPERTY(EditDefaultsOnly)
	float Duration;
};
