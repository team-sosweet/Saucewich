// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "SaucewichGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStateSpawned, class ASaucewichGameState*);

UENUM(BlueprintType)
enum class EGameRule : uint8
{
	Lobby UMETA(DisplayName = "Lobby"),
	MakeSandwich UMETA(DisplayName = "MakeSandwich"),
};

UCLASS()
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

	struct BroadcastGameStateSpawned;
	
private:
	FOnGameStateSpawned OnGameStateSpawned;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActorPool> ActorPoolClass;

	UPROPERTY(Transient)
	AActorPool* ActorPool;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsAutoShot;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Sensitivity;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CorrectionValue;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EGameRule GameRule;
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
