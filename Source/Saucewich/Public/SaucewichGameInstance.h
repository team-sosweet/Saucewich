// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "SaucewichGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStateSpawned, class ASaucewichGameState*);

UENUM(BlueprintType)
enum class EGameRule : uint8
{
	Lobby, MakeSandwich
};

UCLASS(Config=Game)
class SAUCEWICH_API USaucewichGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	USaucewichGameInstance();
	~USaucewichGameInstance();
	
	class AActorPool* GetActorPool();
	ASaucewichGameState* GetGameState() const;

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
	
	UFUNCTION(BlueprintCallable)
	float GetSensitivity() const;

	UFUNCTION(BlueprintCallable, DisplayName="Save Config")
	void BP_SaveConfig() { SaveConfig(); }

	bool IsAutoFire() const { return bAutoFire; }

	struct BroadcastGameStateSpawned;
	
private:
	FOnGameStateSpawned OnGameStateSpawned;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActorPool> ActorPoolClass;

	UPROPERTY(Transient)
	AActorPool* ActorPool;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	float Sensitivity = .5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float CorrectionValue = 1;

	UPROPERTY(Transient, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	EGameRule GameRule;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	uint8 bAutoFire : 1;
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
