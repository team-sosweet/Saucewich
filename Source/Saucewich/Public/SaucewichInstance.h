// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "Engine/EngineTypes.h"
#include "SaucewichInstance.generated.h"

class AWeapon;
class AActorPool;
class ASauceMarker;
class ASaucewichGameMode;

USTRUCT(BlueprintType)
struct SAUCEWICH_API FScoreData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Score;
};

UCLASS()
class SAUCEWICH_API USaucewichInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Saucewich Instance", WorldContext=WorldContextObj))
	static USaucewichInstance* Get(const UObject* WorldContextObj);

	class UUserSettings* GetUserSettings() const { return UserSettings; }
	AActorPool* GetActorPool() const;
	ASauceMarker* GetSauceMarker() const;
	auto&& GetGameModes() const { return GameModes; }
	auto&& GetScoreData(const FName& ID) const { return ScoreData[ID]; }
	ECollisionChannel GetDecalTraceChannel() const { return DecalTraceChannel; }

	UFUNCTION(BlueprintCallable)
	bool PopNetworkError(TEnumAsByte<ENetworkFailure::Type>& Type, FString& Msg);

protected:
	void Init() override;

private:
	void OnNetworkError(UWorld*, class UNetDriver*, ENetworkFailure::Type, const FString&);
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, FScoreData> ScoreData;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<ASauceMarker> SauceMarkerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSoftClassPtr<ASaucewichGameMode>> GameModes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSoftClassPtr<AWeapon>> PrimaryWeapons;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TSoftClassPtr<AWeapon>> SecondaryWeapons;
	
	UPROPERTY(Transient)
	mutable AActorPool* ActorPool;

	UPROPERTY(Transient)
	mutable ASauceMarker* SauceMarker;

	UPROPERTY(Transient)
	UUserSettings* UserSettings;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> DecalTraceChannel;

	struct
	{
		FString Msg;
		ENetworkFailure::Type Type;
		uint8 bOccured : 1;
	} LastNetworkError;
};
