// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Widget/ComponentWidget.h"
#include "MakeSandwichResultComponent.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class SAUCEWICH_API UMakeSandwichResultComponent : public UComponentWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidget(uint8 WinningTeam) const;

	UFUNCTION(BlueprintCallable)
	float GetTimeRemainingForNextGame() const;

protected:
	void NativeOnInitialized() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FName, FText> ResultTexts;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FName, UTexture2D*> ResultTextures;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMaterialInterface* SandwichMaterialParent;
	
	UPROPERTY(Transient)
	UTextBlock* ResultText;

	UPROPERTY(Transient)
	UTextBlock* MyTeamScoreText;

	UPROPERTY(Transient)
	UTextBlock* EnemyTeamScoreText;

	UPROPERTY(Transient)
	UImage* MyTeamSandwich;

	UPROPERTY(Transient)
	UImage* EnemyTeamSandwich;

	UPROPERTY(Transient)
	UImage* MyTeamResultImage;

	UPROPERTY(Transient)
	UImage* EnemyTeamResultImage;

	UPROPERTY(Transient)
	class UUsersInfo* UsersInfo;
	
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* MyTeamSandwichMat;
	
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* EnemyTeamSandwichMat;

	float NextMatchStartTime;
};
