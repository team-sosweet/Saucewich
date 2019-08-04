// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichHUD.h"
#include "AliveHUD.h"
#include "DeathHUD.h"
#include "TpsCharacter.h"
#include "TimerManager.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	AliveWidget = CreateWidget<UAliveHUD>(GetOwningPlayerController(), *AliveWidgetClass);
	DeathWidget = CreateWidget<UDeathHUD>(GetOwningPlayerController(), *DeathWidgetClass);

	DeathWidget->AddToViewport();
	OnSpawn();

	ATpsCharacter* Player = Cast<ATpsCharacter>(GetOwningPawn());
	Player->OnCharacterSpawn.AddDynamic(this, &ASaucewichHUD::OnSpawn);
	Player->OnCharacterDeath.AddDynamic(this, &ASaucewichHUD::OnDeath);
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