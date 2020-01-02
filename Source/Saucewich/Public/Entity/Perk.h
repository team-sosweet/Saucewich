// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "Perk.generated.h"

UCLASS()
class SAUCEWICH_API APerk : public APickup
{
	GENERATED_BODY()

public:
	APerk();
	
	float GetDuration() const { return Duration; }
	auto GetParticle() const { return Particle; }
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Highlight;
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Particle;
	
	UPROPERTY(EditDefaultsOnly)
	float Duration;
};
