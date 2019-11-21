// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UserInfo.generated.h"

class UTextBlock;

UCLASS()
class SAUCEWICH_API UUserInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateInfo(class ASaucewichPlayerState* PlayerState) const;

protected:
	void NativeOnInitialized() override;

private:
	UPROPERTY(Transient)
	UTextBlock* NameText;
	
	UPROPERTY(Transient)
	UTextBlock* ScoreText;
	
	UPROPERTY(Transient)
	UTextBlock* ObjectiveText;
	
	UPROPERTY(Transient)
	UTextBlock* KillText;
	
	UPROPERTY(Transient)
	UTextBlock* DeathText;
};
