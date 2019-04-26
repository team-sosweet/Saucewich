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
	if (!DataTable)
	{
		check(NewWeaponData);
		DataTable = NewWeaponData;

		// TODO: 캐릭터/무기 선택이 구현되면 비동기 로드로 변경
		Mesh->SetStaticMesh(DataTable->Mesh.LoadSynchronous());

		static const FName SlotName{ "Color" };
		ColorDynamicMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex(SlotName)), this);
		Mesh->SetMaterialByName(SlotName, ColorDynamicMaterial);
	}
}

FWeaponData::FWeaponData()
	:BaseClass{ AWeapon::StaticClass() }
{
}

FWeaponData::FWeaponData(const TSubclassOf<AWeapon>& Base)
	:BaseClass{ Base }
{
}
