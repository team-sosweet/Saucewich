// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Widget/ComponentWidget.h"
#include "MakeSandwichResultComponent.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class SAUCEWICH_API UMakeSandwichResultComponent : public UComponentWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetWidget(uint8 WinningTeam) const;
	
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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 WaitTime;
	
	FTimerHandle WaitTimer;
};
