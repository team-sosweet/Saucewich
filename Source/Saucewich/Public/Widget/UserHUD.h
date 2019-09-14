// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Widget/BaseWidget.h"
#include "UserHUD.generated.h"

UCLASS()
class SAUCEWICH_API UUserHUD final : public UBaseWidget
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

	template <class Fn>
	void BindPlayerState(const APawn* InPawn, Fn&& Callback)
	{
		if (const auto PS = InPawn->GetPlayerState<ASaucewichPlayerState>())
		{
			Callback(PS);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, InPawn, Callback]
				{
					BindPlayerState(InPawn, Callback);
				});
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowAngle;

	UPROPERTY(Transient)
	APawn* OwnerPawn;

	UPROPERTY(Transient)
	APawn* LocalPawn;

	float ShowAngleRadian;

	uint8 OwnerTeam;

	uint8 LocalTeam;
};
