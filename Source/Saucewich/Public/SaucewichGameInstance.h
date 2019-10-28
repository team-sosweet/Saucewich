// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "SaucewichGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStateSpawned, class ASaucewichGameState*);

UCLASS(Config=Game)
class SAUCEWICH_API USaucewichGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
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

	void SaveWeaponLoadout(const TArray<TSubclassOf<class AWeapon>>& Loadout)
	{
		WeaponLoadout = Loadout;
		SaveConfig();
	}

	auto& GetGameModes() const { return GameModes; }
	auto& GetWeaponLoadout() const { return WeaponLoadout; }

	bool IsAutoFire() const { return bAutoFire; }

	struct BroadcastGameStateSpawned;

private:
	UPROPERTY(Config)
	TArray<TSubclassOf<class ASaucewichGameMode>> GameModes;
	
	UPROPERTY(Config)
	TArray<TSubclassOf<AWeapon>> WeaponLoadout;
	
	FOnGameStateSpawned OnGameStateSpawned;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActorPool> ActorPoolClass;
	
	UPROPERTY(Transient)
	AActorPool* ActorPool;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	float Sensitivity = .5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float CorrectionValue = 1;

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
