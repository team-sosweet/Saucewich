// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "AliveHUD.h"
#include "TpsCharacter.h"
#include "Weapon.h"
#include "WeaponComponent.h"
#include "TimerManager.h"
#include "SaucewichGameState.h"
#include "SaucewichPlayerState.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanelSlot.h"
#include "Classes/Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMaterialLibrary.h"

void UAliveHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RootWidget = Cast<UPanelWidget>(GetWidgetFromName(TEXT("Root")));
	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_HP")));
	ClipProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_Clip")));
	SubWeaponProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_SubWeapon")));
	SubWeaponButton = Cast<UButton>(GetWidgetFromName(TEXT("Button_SubWeapon")));
	AttackButton = Cast<UButton>(GetWidgetFromName(TEXT("Button_Attack")));

	GameState = GetWorld()->GetGameState<ASaucewichGameState>();

	UCanvasPanelSlot* HPSlot = Cast<UCanvasPanelSlot>(HealthProgressBar->Slot);

	FVector2D HealthBarSize;
	HealthBarSize.X = HPSlot->GetSize().X;
	HealthBarSize.Y = GetHealthBarThickness();

	HPSlot->SetSize(HealthBarSize);

	UWeaponComponent* WeaponComponent = GetOwningPlayerPawn<ATpsCharacter>()->GetWeaponComponent();

	AWeapon* MainWeapon = WeaponComponent->GetWeapon(0);
	AddProgressBarMaterial(ClipProgressBar, MainWeapon->GetIcon(), MainWeapon->GetMask());

	AWeapon* SubWeapon = WeaponComponent->GetWeapon(1);
	AddProgressBarMaterial(SubWeaponProgressBar, SubWeapon->GetIcon(), SubWeapon->GetMask());

	UUserWidget* ChildWidget = CreateWidget(GetOwningPlayer(), *ChildWidgetClass);
	RootWidget->AddChild(ChildWidget);

	BindOnTeamChanged();
}

void UAliveHUD::SetTeamColor(uint8 NewTeam)
{
	MyTeamColor = GameState->GetTeamData(NewTeam).Color;

	uint8 EnemyTeam = (NewTeam == 1) ? 2 : 1;
	EnemyTeamColor = GameState->GetTeamData(NewTeam).Color;

	for (UMaterialInstanceDynamic* Material : Materials)
	{
		Material->SetVectorParameterValue(TEXT("Color"), MyTeamColor);
	}
}

void UAliveHUD::BindOnTeamChanged()
{
	ASaucewichPlayerState* PlayerState = GetOwningPlayerState<ASaucewichPlayerState>();

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
	UMaterialInstanceDynamic* Material =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), IconMaterial);

	Material->SetTextureParameterValue(TEXT("Icon"), Icon);
	Material->SetTextureParameterValue(TEXT("Mask"), Mask);

	ProgressBar->WidgetStyle.FillImage.SetResourceObject(Material);
	ProgressBar->WidgetStyle.BackgroundImage.SetResourceObject(Material);
	
	Materials.Add(Material);
}