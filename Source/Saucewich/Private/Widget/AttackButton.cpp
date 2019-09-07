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
	Super::NativeOnInitialized();

	CoolProgressBar = Cast<UProgressBar>(GetWidgetFromName("ProgressBar_Cool"));
	CoolProgressBar->PercentDelegate.BindDynamic(this, &UAttackButton::GetPercent);
	
	CoolMaterial =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), CoolMaterialParent);

	OnMaterialSpawned();
	
	CoolProgressBar->WidgetStyle.FillImage.SetResourceObject(CoolMaterial);
	CoolProgressBar->WidgetStyle.BackgroundImage.SetResourceObject(CoolMaterial);
	
	const auto Player = GetOwningPlayer();
	const auto HUD = Cast<ASaucewichHUD>(Player->GetHUD());
	
	FOnChangedColorSingle ChangeColorDelegate;
	ChangeColorDelegate.BindDynamic(this, &UAttackButton::OnChangedColor);
	HUD->BindChangedColor(ChangeColorDelegate);

	const auto WeaponComponent = Cast<ATpsCharacter>(Player->GetPawn())->GetWeaponComponent();
	
	FOnEquipWeaponSingle Delegate;
	Delegate.BindDynamic(this, &UAttackButton::OnWeaponChanged);
	WeaponComponent->AddOnEquipWeapon(Delegate);
}

void UAttackButton::OnChangedColor_Implementation(const FLinearColor& MyTeamColor)
{
	CoolMaterial->SetVectorParameterValue(TEXT("Color"), MyTeamColor);
}
