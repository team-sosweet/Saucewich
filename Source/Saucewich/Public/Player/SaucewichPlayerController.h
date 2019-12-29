// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Player/BasePC.h"
#include "Saucewich.h"
#include "SaucewichPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawned, class ASaucewichPlayerState*, PlayerState);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPSSpawnedNative, ASaucewichPlayerState*)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerStateSpawnedSingle, ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawned, class ATpsCharacter*, Character);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCharacterSpawnedSingle, ATpsCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharRespawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReceiveMessage, const FText&, Message, float, Duration, EMsgType, Type);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerController : public ABasePC
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void SetRespawnTimer(float RespawnTime);

	UFUNCTION(BlueprintCallable)
	float GetRemainingRespawnTime() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void Respawn();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRespawn();

	UFUNCTION(BlueprintCallable)
	void SafePlayerState(const FOnPlayerStateSpawnedSingle& Delegate);
	void SafePS(FOnPSSpawnedNative::FDelegate&& Delegate);
	
	UFUNCTION(BlueprintCallable)
	void SafeCharacter(const FOnCharacterSpawnedSingle& Delegate);

	UFUNCTION(NetMulticast, Reliable)
	void PrintMessage(const FText& Message, float Duration, EMsgType Type);

	void SetSessionID(FString&& ID);
	const FString& GetSessionID() const;

	struct BroadcastPlayerStateSpawned;
	struct BroadcastCharacterSpawned;

	UPROPERTY(BlueprintAssignable)
	FOnCharRespawn OnCharRespawn;

	UPROPERTY(BlueprintAssignable)
	FOnCharDied OnCharDied;

	UPROPERTY(BlueprintAssignable)
	FOnReceiveMessage OnReceiveMessage;

protected:
	void BeginPlay() override;
	void InitPlayerState() override;

private:
	bool CanRespawn() const;

	void Ping();
	void OnPingFailed() const;
	void Disconnect() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPing();

	UFUNCTION(Client, Reliable)
	void ClientPing();
	
	FOnPlayerStateSpawned OnPlayerStateSpawned;
	FOnPSSpawnedNative OnPSSpawnedNative;
	FOnCharacterSpawned OnCharacterSpawned;

	FString SessionID;

	FTimerHandle RespawnTimer;
	FTimerHandle PingTimer;

	UPROPERTY(EditDefaultsOnly)
	float PingTimeout = 1;
};

struct ASaucewichPlayerController::BroadcastPlayerStateSpawned
{
private:
	friend ASaucewichPlayerState;
	BroadcastPlayerStateSpawned(ASaucewichPlayerController* Controller, ASaucewichPlayerState* PlayerState)
	{
		Controller->OnPlayerStateSpawned.Broadcast(PlayerState);
		Controller->OnPlayerStateSpawned.Clear();
		Controller->OnPSSpawnedNative.Broadcast(PlayerState);
		Controller->OnPSSpawnedNative.Clear();
	}
};

struct ASaucewichPlayerController::BroadcastCharacterSpawned
{
private:
	friend ATpsCharacter;
	BroadcastCharacterSpawned(ASaucewichPlayerController* Controller, ATpsCharacter* Character)
	{
		Controller->OnCharacterSpawned.Broadcast(Character);
		Controller->OnCharacterSpawned.Clear();
	}
};
