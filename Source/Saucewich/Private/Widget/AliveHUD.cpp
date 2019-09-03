// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/AliveHUD.h"

#include "Classes/Materials/MaterialInstanceDynamic.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "TimerManager.h"

#include "Online/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacter.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponComponent.h"
#include "Widget/FeedBox.h"
#include "Widget/KillFeed.h"

void UAliveHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_HP")));
	ClipProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_Clip")));
	SubWeaponProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar_SubWeapon")));
	SubWeaponButton = Cast<UButton>(GetWidgetFromName(TEXT("Button_SubWeapon")));
	AttackButton = Cast<UBorder>(GetWidgetFromName(TEXT("Button_Attack")));
	KillFeedBox = Cast<UFeedBox>(GetWidgetFromName(TEXT("FeedBox_Kill")));

	GameState = GetWorld()->GetGameState<ASaucewichGameState>();
	GameState->OnPlayerDeath.AddDynamic(this, &UAliveHUD::OnPlayerDeath);

	const auto HPSlot = Cast<UCanvasPanelSlot>(HealthProgressBar->Slot);

	FVector2D HealthBarSize;
	HealthBarSize.X = HPSlot->GetSize().X;
	HealthBarSize.Y = GetHealthBarThickness();

	HPSlot->SetSize(HealthBarSize);

	AddProgressBarMaterial(ClipProgressBar);
	AddProgressBarMaterial(SubWeaponProgressBar);

	const auto WeaponComponent = GetOwningPlayerPawn<ATpsCharacter>()->GetWeaponComponent();
	WeaponComponent->AddOnEquipWeapon(this, &UAliveHUD::OnEquipWeapon, true);

	BindOnTeamChanged();
}

void UAliveHUD::OnEquipWeapon(AWeapon* const Weapon)
{
	/* TODO: 갈아엎기
	
	const auto WeaponSlot = Weapon->GetSlot();
	const auto Material = Materials[WeaponSlot];
	
	Material->SetTextureParameterValue(TEXT("Icon"), Weapon->GetIcon());
	Material->SetTextureParameterValue(TEXT("Mask"), Weapon->GetMask());

	auto ProgressBar = ClipProgressBar;
	auto ProgressBarVisibility = ESlateVisibility::SelfHitTestInvisible;

	if (WeaponSlot == 1)
	{
		ProgressBar = SubWeaponProgressBar;
		ProgressBarVisibility = ESlateVisibility::Visible;
	}

	ProgressBar->SetVisibility(ProgressBarVisibility);
	*/
}

void UAliveHUD::OnPlayerDeath(ASaucewichPlayerState* Victim,
	ASaucewichPlayerState* Attacker, AActor* Inflictor)
{
	KillFeedBox->MakeNewFeed(FKillFeedContent(Victim, Attacker, Inflictor));
}

void UAliveHUD::SetTeamColor(const uint8 NewTeam)
{
	const uint8 EnemyTeam = (NewTeam == 1) ? 2 : 1;
	
	MyTeamColor = GameState->GetTeamData(NewTeam).Color;
	EnemyTeamColor = GameState->GetTeamData(EnemyTeam).Color;

	for (const auto Material : Materials)
	{
		Material->SetVectorParameterValue(TEXT("Color"), MyTeamColor);
	}
}

void UAliveHUD::BindOnTeamChanged()
{
	const auto PlayerState = GetOwningPlayerState<ASaucewichPlayerState>();

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

void UAliveHUD::AddProgressBarMaterial(UProgressBar* ProgressBar)
{
	const auto Material =
		UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), IconMaterial);

	ProgressBar->SetVisibility(ESlateVisibility::Hidden);
	ProgressBar->WidgetStyle.FillImage.SetResourceObject(Material);
	ProgressBar->WidgetStyle.BackgroundImage.SetResourceObject(Material);

	Materials.Add(Material);
}
