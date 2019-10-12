// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Saucewich.h"
#include "SaucewichPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawned, class ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerStateSpawnedSingle, ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawned, class ATpsCharacter*, Character);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCharacterSpawnedSingle, ATpsCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharRespawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReceiveMessage, const FText&, Message, float, Duration, EMsgType, Type);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerController final : public APlayerController
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
	
	UFUNCTION(BlueprintCallable)
	void SafeCharacter(const FOnCharacterSpawnedSingle& Delegate);

	UFUNCTION(NetMulticast, Reliable)
	void PrintMessage(const FText& Message, float Duration, EMsgType Type);

	struct BroadcastPlayerStateSpawned;
	struct BroadcastCharacterSpawned;

	UPROPERTY(BlueprintAssignable)
	FOnCharRespawn OnCharRespawn;

	UPROPERTY(BlueprintAssignable)
	FOnReceiveMessage OnReceiveMessage;

protected:
	void BeginPlay() override;
	void InitPlayerState() override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnPingFailed();

private:
	bool CanRespawn() const;

	void Ping();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPing();

	UFUNCTION(Client, Reliable)
	void ClientPing();
	
	FOnPlayerStateSpawned OnPlayerStateSpawned;
	FOnCharacterSpawned OnCharacterSpawned;

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
	}
};

struct ASaucewichPlayerController::BroadcastCharacterSpawned
{
private:
	friend ATpsCharacter;
	BroadcastCharacterSpawned(ASaucewichPlayerController* Controller, ATpsCharacter* Character)
	{
		Controller->OnCharacterSpawned.Broadcast(Character);
	}
};
