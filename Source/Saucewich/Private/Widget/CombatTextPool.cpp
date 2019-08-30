// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/CombatTextPool.h"

#include "Player/TpsCharacter.h"
#include "Widget/CombatText.h"

void UCombatTextPool::NewCombatText(const float Damage, ATpsCharacter* DamagedActor)
{
	UCombatText* CombatText;
	
	if (Items.empty())
	{
		CombatText = CreateWidget<UCombatText>(Owner, ItemClass);
		CombatText->OnRemove.BindUObject(this, &UCombatTextPool::Arrange);
	}
	else
	{
		CombatText = Items.front();
		Items.pop();
	}

	CombatText->ViewCombatText(Damage, DamagedActor);
}

void UCombatTextPool::Arrange(UCombatText* Widget)
{
	Items.push(Widget);
}
