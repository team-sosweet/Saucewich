// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SaucewichPlayerState.generated.h"

UCLASS()
class SAUCEWICH_API ASaucewichPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	uint8 GetTeam() const { return Team; }

	UFUNCTION(NetMulticast, Reliable)
	void SetTeam(uint8 NewTeam);

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
