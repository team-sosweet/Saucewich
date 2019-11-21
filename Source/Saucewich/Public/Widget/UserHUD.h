// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
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
	ESlateVisibility GetHUDVisibility();

	UFUNCTION()
	void OnLocalCharacterSpawned(ATpsCharacter* Character);
	
	UFUNCTION()
	void OnOwnerTeamChanged(uint8 NewTeam);

	UFUNCTION()
	void OnLocalTeamChanged(uint8 NewTeam);

	UFUNCTION()
	void OnSpawn();

	UFUNCTION()
	void OnDeath();
	
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

	UPROPERTY(Transient)
	ATpsCharacter* OwnerPawn;

	UPROPERTY(Transient)
	ATpsCharacter* LocalPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ShowAngle;

	class AGameState* GameState;
	
	float ShowAngleRadian;

	uint8 OwnerTeam;

	uint8 LocalTeam;

	bool IsDead;
};
