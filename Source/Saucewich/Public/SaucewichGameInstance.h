// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "HttpGameInstance.h"
#include "SaucewichGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStateSpawned, class ASaucewichGameState*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeAccount);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRespondGetGameCode, const FString&)

USTRUCT(Atomic, BlueprintType)
struct FAccount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Exp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float KillDeath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WinLose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AccessToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefreshToken;
};

UCLASS(Config=Game)
class SAUCEWICH_API USaucewichGameInstance final : public UHttpGameInstance
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

	UFUNCTION(BlueprintCallable)
	void SetAccount(const FAccount& InAccount)
	{
		Account = InAccount;
		bHaveAccount = true;
		SaveConfig();
		OnChangeAccount.Broadcast();
	}

	UFUNCTION(BlueprintCallable)
	void ClearAccount()
	{
		bHaveAccount = false;
		SaveConfig();
		OnChangeAccount.Broadcast();
	}
	
	void SaveWeaponLoadout(const TArray<TSubclassOf<class AWeapon>>& Loadout)
	{
		WeaponLoadout = Loadout;
		SaveConfig();
	}

	auto& GetGameModes() const { return GameModes; }
	auto& GetWeaponLoadout() const { return WeaponLoadout; }

	bool IsAutoFire() const { return bAutoFire; }

	// void Callback(const FString&)
	template <class Fn>
	void GetGameCode(Fn&& Callback)
	{
		if (!GameCode.IsEmpty())
		{
			Callback(GameCode);
		}
		else
		{
			OnRespondGetGameCode.AddLambda(Forward<Fn>(Callback));
			FOnResponded OnResponded;
			OnResponded.BindDynamic(this, &USaucewichGameInstance::RespondGetGameCode);
			GetRequest(FString::Printf(TEXT("room/game/%d"), Port), {}, OnResponded);
		}
	}

	struct BroadcastGameStateSpawned;

	UPROPERTY(BlueprintAssignable)
	FOnChangeAccount OnChangeAccount;

protected:
	void BeginDestroy() override;
	
private:
	UFUNCTION()
	void RespondGetGameCode(bool bIsSuccess, int32 Code, FJson Json);

	UPROPERTY(Config)
	TArray<TSubclassOf<class ASaucewichGameMode>> GameModes;
	
	UPROPERTY(Config)
	TArray<TSubclassOf<AWeapon>> WeaponLoadout;
	
	FOnGameStateSpawned OnGameStateSpawned;
	FOnRespondGetGameCode OnRespondGetGameCode;

	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FAccount Account;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FString GameCode;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FString GamePort;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FString GameServerAddress;
	
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

	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bHaveAccount : 1;
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
