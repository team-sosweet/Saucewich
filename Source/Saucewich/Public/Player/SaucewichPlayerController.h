// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SaucewichPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawned, class ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerStateSpawnedSingle, ASaucewichPlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSpawned, class ATpsCharacter*, Character);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCharacterSpawnedSingle, ATpsCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharRespawn);

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
	void PrintMessage(FName MessageID, float Duration);

	void InitMessage();

	void SetID(const FString& NewID, const FString& NewToken) { ID = NewID; Token = NewToken; }
	auto& GetID() const { return ID; }
	auto& GetToken() const { return Token; }

	struct BroadcastPlayerStateSpawned;
	struct BroadcastCharacterSpawned;

	UPROPERTY(BlueprintAssignable)
	FOnCharRespawn OnCharRespawn;
	
private:
	bool CanRespawn() const;
	void ClearMessage() { Message = FText::GetEmpty(); }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FText Message;

	FOnPlayerStateSpawned OnPlayerStateSpawned;
	FOnCharacterSpawned OnCharacterSpawned;

	FTimerHandle RespawnTimer;
	FTimerHandle MessageTimer;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString ID;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString Token;
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
