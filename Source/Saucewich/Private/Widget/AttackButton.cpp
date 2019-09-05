// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/AttackButton.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Player/SaucewichHUD.h"
#include "Player/TpsCharacter.h"
#include "Weapon/WeaponComponent.h"

void UAttackButton::NativeOnInitialized()
{
	CoolProgressBar = Cast<UProgressBar>(GetWidgetFromName("ProgressBar_Cool"));
	CoolProgressBar->PercentDelegate.BindDynamic(this, &UAttackButton::GetPercent);
	
	CoolMaterial =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), CoolMaterialParent);

	CoolProgressBar->WidgetStyle.FillImage.SetResourceObject(CoolMaterial);
	CoolProgressBar->WidgetStyle.BackgroundImage.SetResourceObject(CoolMaterial);

	const auto Player = GetOwningPlayer();
	const auto HUD = Cast<ASaucewichHUD>(Player->GetHUD());
	HUD->OnChangeColor.AddDynamic(this, &UAttackButton::OnChangeColor);

	const auto WeaponComponent = Cast<ATpsCharacter>(Player->GetPawn())->GetWeaponComponent();
	WeaponComponent->OnEquipWeapon.AddDynamic(this, &UAttackButton::OnWeaponChanged);
}

void UAttackButton::OnChangeColor(const FLinearColor MyTeamColor)
{
	CoolMaterial->SetVectorParameterValue(TEXT("Team Color"), MyTeamColor);
}
