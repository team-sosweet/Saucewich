// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichHUD.h"
#include "BaseHUD.h"
#include "DeathHUD.h"
#include "TpsCharacter.h"
#include "TimerManager.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	SetHUD(AliveWidget, DeathWidget);
	AliveWidget->AddToViewport();

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