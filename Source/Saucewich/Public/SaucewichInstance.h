// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
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
	AActorPool* GetActorPool() const;
	ASauceMarker* GetSauceMarker() const;
	auto&& GetGameModes() const { return GameModes; }
	auto&& GetScoreData(const FName& ID) const { return ScoreData[ID]; }

private:
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
};
