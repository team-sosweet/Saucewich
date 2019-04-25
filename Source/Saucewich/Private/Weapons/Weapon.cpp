// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ActorPoolComponent.h"
#include "SaucewichCharacter.h"

AWeapon::AWeapon()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") }
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = Mesh;
	SetActorEnableCollision(false);
}

void AWeapon::SetColor(const FLinearColor& Color)
{
	ColorDynamicMaterial->SetVectorParameterValue("Base Color", Color);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto Character{ GetOwner() })
	{
		Role = Character->Role;
		CopyRemoteRoleFrom(Character);
	}
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

void AWeapon::Equip(const FWeaponData* NewWeaponData)
{
	if (NewWeaponData && !DataTable)
	{
		DataTable = NewWeaponData;
		Mesh->SetStaticMesh(DataTable->Mesh.LoadSynchronous());
		ColorDynamicMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex("Color")), this);
		Mesh->SetMaterialByName("Color", ColorDynamicMaterial);
	}
}

UClass* FWeaponData::GetBaseClass() const
{
	return AWeapon::StaticClass();
}
