// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Widget/BaseWidget.h"
#include "AliveWidget.generated.h"

UCLASS()
class SAUCEWICH_API UAliveWidget final : public UBaseWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

	UFUNCTION()
	void OnPlayerStateSpawned(class ASaucewichPlayerState* PlayerState);
	
	UFUNCTION()
	void OnPlayerDeath(class ASaucewichPlayerState* Victim,
		ASaucewichPlayerState* Attacker, AActor* Inflictor);

	UFUNCTION()
	void OnScoreAdded(FName ScoreID, int32 ActualScore);

	UPROPERTY(Transient)
	class UFeedBox* KillFeedBox;

	UPROPERTY(Transient)
	class UFeedBox* ScoreFeedBox;

	class ASaucewichGameState* GameState;
};
