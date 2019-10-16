// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UserInfo.generated.h"

UCLASS()
class SAUCEWICH_API UUserInfo : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	void UpdateInfo(class ASaucewichPlayerState* PlayerState);

private:
	UPROPERTY(Transient)
	class UTextBlock* NameText;
	
	UPROPERTY(Transient)
	UTextBlock* ScoreText;
	
	UPROPERTY(Transient)
	UTextBlock* ObjectiveText;
	
	UPROPERTY(Transient)
	UTextBlock* KillText;
	
	UPROPERTY(Transient)
	UTextBlock* DeathText;
	
	class ASaucewichGameState* GameState;
};
