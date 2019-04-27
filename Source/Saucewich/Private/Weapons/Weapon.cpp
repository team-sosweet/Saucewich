// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ActorPoolComponent.h"
#include "SaucewichCharacter.h"
#include "UnrealNetwork.h"

DECLARE_LOG_CATEGORY_CLASS(LogWeapon, Log, All)

AWeapon::AWeapon()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") }
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = Mesh;
	SetActorEnableCollision(false);
}

static const FName ColorMaterialParameterName{ "Base Color" };

void AWeapon::SetColor(const FLinearColor& Color)
{
	ColorDynamicMaterial->SetVectorParameterValue(ColorMaterialParameterName, Color);
}

FLinearColor AWeapon::GetColor() const
{
	FLinearColor Color;
	ColorDynamicMaterial->GetVectorParameterValue(ColorMaterialParameterName, Color);
	return Color;
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (const auto Character{ GetOwner() })
	{
		Role = Character->Role;
		CopyRemoteRoleFrom(Character);
	}

	static const FName SlotName{ "Color" };
	ColorDynamicMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex(SlotName)), this);
	Mesh->SetMaterialByName(SlotName, ColorDynamicMaterial);
}

void AWeapon::SetActivated(const bool bActive)
{
	SetActorTickEnabled(bActive);
	SetActorHiddenInGame(!bActive);
}

bool AWeapon::CanAttack() const
{
	const auto Character{ GetInstigator<ASaucewichCharacter>() };
	return Character && Character->Alive();
}
