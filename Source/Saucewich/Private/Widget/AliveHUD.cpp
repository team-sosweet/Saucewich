// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/AliveHUD.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Classes/Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "TimerManager.h"

#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacter.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponComponent.h"
#include "Online/SaucewichGameState.h"

void UAliveHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_HP")));
	ClipProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_Clip")));
	SubWeaponProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_SubWeapon")));
	SubWeaponButton = Cast<UButton>(GetWidgetFromName(TEXT("Button_SubWeapon")));
	AttackButton = Cast<UBorder>(GetWidgetFromName(TEXT("Button_Attack")));

	GameState = GetWorld()->GetGameState<ASaucewichGameState>();

	auto HPSlot = Cast<UCanvasPanelSlot>(HealthProgressBar->Slot);

	FVector2D HealthBarSize;
	HealthBarSize.X = HPSlot->GetSize().X;
	HealthBarSize.Y = GetHealthBarThickness();

	HPSlot->SetSize(HealthBarSize);

	const auto WeaponComponent = GetOwningPlayerPawn<ATpsCharacter>()->GetWeaponComponent();

	const auto MainWeapon = WeaponComponent->GetWeapon(0);
	AddProgressBarMaterial(ClipProgressBar, MainWeapon->GetIcon(), MainWeapon->GetMask());

	const auto SubWeapon = WeaponComponent->GetWeapon(1);
	AddProgressBarMaterial(SubWeaponProgressBar, SubWeapon->GetIcon(), SubWeapon->GetMask());

	BindOnTeamChanged();
}

void UAliveHUD::SetTeamColor(const uint8 NewTeam)
{
	MyTeamColor = GameState->GetTeamData(NewTeam).Color;

	const uint8 EnemyTeam = (NewTeam == 1) ? 2 : 1;
	EnemyTeamColor = GameState->GetTeamData(EnemyTeam).Color;

	for (auto Material : Materials)
	{
		Material->SetVectorParameterValue(TEXT("Color"), MyTeamColor);
	}
}

void UAliveHUD::BindOnTeamChanged()
{
	auto PlayerState = GetOwningPlayerState<ASaucewichPlayerState>();

	if (PlayerState)
	{
		PlayerState->OnTeamChangedDelegate.AddDynamic(this, &UAliveHUD::SetTeamColor);
		SetTeamColor(PlayerState->GetTeam());
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAliveHUD::BindOnTeamChanged);
	}
}

void UAliveHUD::AddProgressBarMaterial(UProgressBar* ProgressBar, UTexture* Icon, UTexture* Mask)
{
	auto Material =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), IconMaterial);

	Material->SetTextureParameterValue(TEXT("Icon"), Icon);
	Material->SetTextureParameterValue(TEXT("Mask"), Mask);

	ProgressBar->WidgetStyle.FillImage.SetResourceObject(Material);
	ProgressBar->WidgetStyle.BackgroundImage.SetResourceObject(Material);

	Materials.Add(Material);
}
