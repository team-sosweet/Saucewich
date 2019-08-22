// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Player/SaucewichHUD.h"

#include "CoreMinimal.h"
#include "TimerManager.h"

#include "Player/TpsCharacter.h"
#include "Widget/AliveHUD.h"
#include "Widget/DeathHUD.h"

void ASaucewichHUD::BeginPlay()
{
	Super::BeginPlay();

	AliveWidget = CreateWidget<UAliveHUD>(GetOwningPlayerController(), *AliveWidgetClass);
	DeathWidget = CreateWidget<UDeathHUD>(GetOwningPlayerController(), *DeathWidgetClass);
	
	auto Player = Cast<ATpsCharacter>(GetOwningPawn());
	Player->OnCharacterSpawn.AddDynamic(this, &ASaucewichHUD::OnSpawn);
	Player->OnCharacterDeath.AddDynamic(this, &ASaucewichHUD::OnDeath);

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