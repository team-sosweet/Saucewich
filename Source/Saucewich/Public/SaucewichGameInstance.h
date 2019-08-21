// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "SaucewichGameInstance.generated.h"

DECLARE_EVENT_OneParam(USaucewichGameInstance, FOnGameStateReady, class ASaucewichGameState*);

UCLASS()
class SAUCEWICH_API USaucewichGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	USaucewichGameInstance();
	class AActorPool* GetActorPool();
	ASaucewichGameState* GetGameState() const;

	UFUNCTION(BlueprintCallable)
	float GetSensitivity() const;

	template <class Fn>
	void SafeGameState(Fn&& Func)
	{
		if (const auto GS = GetGameState())
		{
			Func(GS);
		}
		else
		{
			OnGameStateReady.AddLambda(Forward<Fn>(Func));
			NotifyWhenGameStateReady();
		}
	}
	
	FOnGameStateReady OnGameStateReady;

private:
	void CheckGameState();
	void NotifyWhenGameStateReady();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<AActorPool> ActorPoolClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	AActorPool* ActorPool;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsAutoShot;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Sensitivity;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CorrectionValue;
};
