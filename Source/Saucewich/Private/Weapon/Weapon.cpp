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
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = Mesh;
}

void AWeapon::Init()
{
	if (const auto MyOwner = GetOwner())
	{
		if (const auto Character = Cast<ATpsCharacter>(MyOwner))
		{
			Role = MyOwner->Role;
			const auto WeaponComponent = Character->GetWeaponComponent();
			WeaponComponent->OnEquipWeapon.Broadcast(this);
			WeaponComponent->GetActiveWeapon() == this ? Deploy() : Holster();
			if (Character->IsInvincible()) BeTranslucent();
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

	const auto NumMat = Mesh->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		Mesh->CreateDynamicMaterialInstance(i);
	}
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
	if (const auto Mat = Mesh->GetMaterial(Mesh->GetMaterialIndex("TeamColor")))
		Mat->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

void AWeapon::SetColor(const FLinearColor& NewColor)
{
	if (const auto Mat = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(Mesh->GetMaterialIndex("TeamColor"))))
		Mat->SetVectorParameterValue("Color", NewColor);
}

void AWeapon::BeTranslucent()
{
	if (bTransl) return;

	for (const auto Mat : Mesh->GetMaterials())
	{
		static_cast<UMaterialInstanceDynamic*>(Mat)->BlendMode = BLEND_Translucent;
	}

	bTransl = true;
}

void AWeapon::BeOpaque()
{
	if (!bTransl) return;

	for (const auto Mat : Mesh->GetMaterials())
	{
		static_cast<UMaterialInstanceDynamic*>(Mat)->BlendMode = BLEND_Opaque;
	}

	bTransl = false;
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
