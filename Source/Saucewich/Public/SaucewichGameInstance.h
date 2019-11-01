// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "SaucewichGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStateSpawned, class ASaucewichGameState*)

UCLASS()
class SAUCEWICH_API USaucewichGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	template <class Fn>
	void SafeGameState(Fn&& Func)
	{
		if (const auto GS = GetGameState())
		{
			Func(GS);
		}
		else
		{
			OnGameStateSpawned.AddLambda(Forward<Fn>(Func));
		}
	}
	
	struct BroadcastGameStateSpawned;

private:
	ASaucewichGameState* GetGameState() const;

	FOnGameStateSpawned OnGameStateSpawned;
};

struct USaucewichGameInstance::BroadcastGameStateSpawned
{
private:
	friend ASaucewichGameState;
	BroadcastGameStateSpawned(USaucewichGameInstance* GI, ASaucewichGameState* GS)
	{
		GI->OnGameStateSpawned.Broadcast(GS);
		GI->OnGameStateSpawned.Clear();
	}
};
