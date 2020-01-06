// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "DSDefGM.generated.h"

UCLASS()
class SAUCEWICH_API ADSDefGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADSDefGM();
	void BeginStartGame() { bStartGame = true; }

protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

private:
	void StartGame() const;
	
	TAtomic<bool> bStartGame;
};
