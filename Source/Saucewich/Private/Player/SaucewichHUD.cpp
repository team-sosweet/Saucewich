// Copyright 2019 Team Sosweet. All Rights Reserved.

// 구현하는 클래스의 헤더
#include "Player/SaucewichHUD.h"

// 엔진 헤더
#include "CoreMinimal.h"
#include "TimerManager.h"

//Saucewich 헤더
#include "Player/TpsCharacter.h"
#include "Widget/AliveHUD.h"
#include "Widget/DeathHUD.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	auto Player = Cast<ATpsCharacter>(GetOwningPawn());
	Player->OnCharacterSpawn.AddDynamic(this, &ASaucewichHUD::OnSpawn);
	Player->OnCharacterDeath.AddDynamic(this, &ASaucewichHUD::OnDeath);

	AliveWidget = CreateWidget<UAliveHUD>(GetOwningPlayerController(), *AliveWidgetClass);
	DeathWidget = CreateWidget<UDeathHUD>(GetOwningPlayerController(), *DeathWidgetClass);

	OnSpawn();
}

void ASaucewichHUD::OnSpawn()
{
	DeathWidget->RemoveFromParent();
	AliveWidget->AddToViewport();
}

void ASaucewichHUD::OnDeath()
{
	AliveWidget->RemoveFromParent();

	GetWorldTimerManager().SetTimer(DeathWidgetTimer, [this]
	{
		DeathWidget->AddToViewport();
	}, DeathWidgetDelay, false);
}