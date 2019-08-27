// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameModeDependentLevelActor.generated.h"

/**
 * 특정 게임 모드에 종속적인, 레벨에 배치되는 액터입니다.
 * 대표적으로 샌드위치 만들기 모드의 재료 스포너가 해당됩니다.
 */
UCLASS()
class SAUCEWICH_API AGameModeDependentLevelActor : public AActor
{
	GENERATED_BODY()
	
protected:
	// 액터 제거가 여기서 일어나므로, 오버라이드를 할 경우 반드시 IsPendingKill() 체크를 하시기 바랍니다.
	void BeginPlay() override;

private:
	// 이 액터가 사용될 게임 스테이트를 지정합니다.
	// 만약 현재 게임 스테이트가 이것이 아니면 액터가 제거됩니다.
	// 비워두면 액터가 제거되지 않습니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGameStateBase> GameStateClass;
};
