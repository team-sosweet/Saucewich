// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SaucewichPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamChanged);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	uint8 GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable)
	void SetTeam(uint8 NewTeam);

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_Team();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Team, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;

	UPROPERTY(BlueprintAssignable)
	FOnTeamChanged OnTeamChanged;
};
