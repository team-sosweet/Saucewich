// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserHUD.generated.h"

UCLASS()
class SAUCEWICH_API UUserHUD final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void Init(APawn* InOwnerPawn);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInit(class ASaucewichPlayerState* PlayerState);
	
private:
	UFUNCTION()
	ESlateVisibility GetHUDVisibility();
	
	UFUNCTION()
	void OnOwnerTeamChanged(uint8 NewTeam);

	UFUNCTION()
	void OnLocalTeamChanged(uint8 NewTeam);
	
	void BindPlayerState(const APawn* InPawn, const TFunction<void(ASaucewichPlayerState*)>& Callback);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowAngle;
	
	APawn* OwnerPawn;

	APawn* LocalPawn;

	float ShowAngleRadian;

	uint8 OwnerTeam;

	uint8 LocalTeam;
};
