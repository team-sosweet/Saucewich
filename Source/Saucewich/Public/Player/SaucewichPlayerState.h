// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SaucewichPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, uint8, NewTeam);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	uint8 GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable)
	void SetTeam(uint8 NewTeam);

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChangedDelegate;

protected:
	UFUNCTION() void OnTeamChanged() const;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing=OnTeamChanged, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
