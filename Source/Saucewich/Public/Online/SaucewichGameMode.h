// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "SaucewichGameMode.generated.h"

UCLASS()
class SAUCEWICH_API ASaucewichGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void SetPlayerDefaults(APawn* PlayerPawn) override;
	FString InitNewPlayer(APlayerController* PC, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	UFUNCTION(BlueprintImplementableEvent)
	void GiveWeapons(class ATpsCharacter* Character);

private:
	class ASaucewichGameState* State;
};
