// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/Pickup.h"
#include "Perk.generated.h"

UCLASS()
class SAUCEWICH_API APerk final : public APickup
{
	GENERATED_BODY()

public:
	float GetDuration() const { return Duration; }
	auto GetParticle() const { return Particle; }
	
protected:
	void BePickedUp_Implementation(ATpsCharacter* By) override;

private:
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Particle;
	
	UPROPERTY(EditDefaultsOnly)
	float Duration;
};