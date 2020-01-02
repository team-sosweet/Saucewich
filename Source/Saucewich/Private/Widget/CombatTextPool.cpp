// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/CombatTextPool.h"

#include "Player/TpsCharacter.h"
#include "Widget/CombatText.h"

void UCombatTextPool::NewCombatText(const float Damage, ATpsCharacter* DamagedActor)
{
	UCombatText* CombatText;
	
	if (Items.Num() == 0)
	{
		CombatText = CreateWidget<UCombatText>(Owner, ItemClass);
		CombatText->OnRemove.BindUObject(this, &UCombatTextPool::Arrange);
	}
	else
	{
		CombatText = Items.Pop(false);
	}

	CombatText->ViewCombatText(Damage, DamagedActor);
}

void UCombatTextPool::Arrange(UCombatText* Widget)
{
	Items.Push(Widget);
}
