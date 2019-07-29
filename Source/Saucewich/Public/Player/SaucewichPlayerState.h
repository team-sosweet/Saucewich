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

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated, Transient, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
