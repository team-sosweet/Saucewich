// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "TpsCharacter.h"
#include "WeaponComponent.h"

AWeapon::AWeapon()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") }
{
	bReplicates = true;
	bAlwaysRelevant = true;
	
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = Mesh;
}

void AWeapon::Init()
{
	if (const auto MyOwner = GetOwner())
	{
		if (const auto Character = Cast<ATpsCharacter>(MyOwner))
		{
			Owner = Character;
			Role = Owner->Role;
			Owner->GetWeaponComponent()->GetActiveWeapon() == this ? Deploy() : Holster();
			Material->SetVectorParameterValue("Color", Owner->GetTeamColor());
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AWeapon::Init);
	}
}

void AWeapon::OnRep_Equipped()
{
	if (bEquipped) Deploy();
	else Holster();
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Material = Mesh->CreateDynamicMaterialInstance(FMath::Max(Mesh->GetMaterialIndex("TeamColor"), 0));
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, bEquipped);
}

void AWeapon::OnActivated()
{
	Init();
}

void AWeapon::OnReleased()
{
	Holster();
}

bool AWeapon::IsVisible() const
{
	return Mesh->bVisible;
}

void AWeapon::SetVisibility(const bool bNewVisibility) const
{
	Mesh->SetVisibility(bNewVisibility);
}

FLinearColor AWeapon::GetColor() const
{
	FLinearColor Color;
	Material->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

void AWeapon::SetColor(const FLinearColor& NewColor)
{
	Material->SetVectorParameterValue("Color", NewColor);
}

ATpsCharacter* AWeapon::GetCharacter() const
{
	return Owner;
}

void AWeapon::Deploy()
{
	Mesh->SetVisibility(true);
	if (HasAuthority())
	{
		bEquipped = true;
	}
}

void AWeapon::Holster()
{
	Mesh->SetVisibility(false);
	if (HasAuthority())
	{
		bEquipped = false;
	}
}
