// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SaucewichPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawned, class ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerStateSpawnedSingle, ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawned, class ATpsCharacter*, Character);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCharacterSpawnedSingle, ATpsCharacter*, Character);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void SetRespawnTimer(float RespawnTime);

	UFUNCTION(BlueprintCallable)
	float GetRemainingRespawnTime() const;

	void Respawn();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRespawn();

	UFUNCTION(BlueprintCallable)
	void SafePlayerState(const FOnPlayerStateSpawnedSingle& Delegate);
	
	UFUNCTION(BlueprintCallable)
	void SafeCharacter(const FOnCharacterSpawnedSingle& Delegate);

	UFUNCTION(NetMulticast, Reliable)
	void PrintMessage(FName MessageID, float Duration);
	
	struct BroadcastPlayerStateSpawned;
	struct BroadcastCharacterSpawned;

private:
	void ClearMessage() { Message = FText::GetEmpty(); }
	
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Message;

	FOnPlayerStateSpawned OnPlayerStateSpawned;
	FOnCharacterSpawned OnCharacterSpawned;
	
	FTimerHandle RespawnTimer;
	FTimerHandle MessageTimer;
};

struct ASaucewichPlayerController::BroadcastPlayerStateSpawned
{
private:
	friend ASaucewichPlayerState;
	BroadcastPlayerStateSpawned(ASaucewichPlayerController* Controller, ASaucewichPlayerState* PlayerState)
	{
		Controller->OnPlayerStateSpawned.Broadcast(PlayerState);
		Controller->OnPlayerStateSpawned.Clear();
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
