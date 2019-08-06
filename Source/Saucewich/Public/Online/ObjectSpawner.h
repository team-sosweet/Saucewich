// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "ObjectSpawner.generated.h"

/**
 * 어떤 액터가 지정된 게임 모드에서만 스폰되도록 하는 스포너입니다.
 * BeginPlay 호출시 바로 스폰됩니다.
 */
UCLASS(Abstract)
class SAUCEWICH_API AObjectSpawner : public AActor
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

private:
	// 스폰할 액터입니다.
	// 리플리케이트가 꺼져있는 액터는 서버에서만 스폰됩니다.
	UPROPERTY(EditInstanceOnly)
	TSubclassOf<AActor> ActorToSpawn;

	// 액터를 스폰할 게임 모드입니다.
	UPROPERTY(EditInstanceOnly)
	TSubclassOf<class AGameModeBase> GameMode;
};
