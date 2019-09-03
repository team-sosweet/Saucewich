// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "TimerManager.h"

#include "Player/TpsCharacter.h"
#include "Widget/AliveHUD.h"
#include "Widget/DeathHUD.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	const auto PC = GetOwningPlayerController();
	const auto Pawn = Cast<ATpsCharacter>(PC->GetPawn());
	if (!Pawn) return;
	
	Pawn->OnCharacterSpawn.AddDynamic(this, &ASaucewichHUD::OnSpawn);
	Pawn->OnCharacterDeath.AddDynamic(this, &ASaucewichHUD::OnDeath);

	AliveWidget = CreateWidget<UAliveHUD>(PC, AliveWidgetClass);
	DeathWidget = CreateWidget<UDeathHUD>(PC, DeathWidgetClass);
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