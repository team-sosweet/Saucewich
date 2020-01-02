// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Widget/CompositeWidget.h"
#include "AliveWidget.generated.h"

enum class EMsgType : uint8;
class UFeedBox;

UCLASS()
class SAUCEWICH_API UAliveWidget : public UCompositeWidget
{
	GENERATED_BODY()

protected:
	void NativeOnInitialized() override;

private:
	UFUNCTION()
	void OnPlayerStateSpawned(class ASaucewichPlayerState* PlayerState);
	
	UFUNCTION()
	void OnPlayerDeath(class ASaucewichPlayerState* Victim, ASaucewichPlayerState* Attacker, AActor* Inflictor);

	UFUNCTION()
	void OnScoreAdded(FName ScoreID, int32 ActualScore);

	UFUNCTION()
	void PrintMessage(const FText& Message, float Duration, EMsgType Type);
	void ClearCenterMessage() const;

	UFeedBox* KillFeedBox;
	UFeedBox* ScoreFeedBox;
	UFeedBox* MessageFeedBox;
	class UTextBlock* CenterText;

	class ASaucewichGameState* GameState;

	FTimerHandle CenterTextTimer;
};
