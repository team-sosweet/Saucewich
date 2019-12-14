// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widget/BaseWidget.h"
#include "UserHUD.generated.h"

UCLASS()
class SAUCEWICH_API UUserHUD : public UBaseWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void Init(class ATpsCharacter* InOwnerPawn);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInit(class ASaucewichPlayerState* PlayerState);
	
private:
	UFUNCTION()
	void InitPlayerState(ASaucewichPlayerState* PlayerState);
	
	UFUNCTION()
	ESlateVisibility GetHUDVisibility();

	UFUNCTION()
	void OnOwnerTeamChanged(uint8 NewTeam);

	UFUNCTION()
	void OnLocalTeamChanged(uint8 NewTeam);

	UFUNCTION()
	void OnSpawn();

	UFUNCTION()
	void OnDeath();
	
	UPROPERTY(Transient)
	ATpsCharacter* OwnerPawn;

	UPROPERTY(Transient)
	ATpsCharacter* LocalPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowAngle;
	
	float ShowAngleRadian;
	uint8 OwnerTeam;
	uint8 LocalTeam;
	uint8 IsDead : 1;
};
