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

void AWeapon::OnRep_DataTableRowName()
{
	if (DataTable)
	{
		Equip(DataTable->FindRow<FWeaponData>(DataTableRowName, TEXT(__FUNCTION__)), DataTableRowName);
	}
	else
	{
		UE_LOG(LogWeapon, Error, TEXT(__FUNCTION__" : Failed to load weapon data through replication. DataTable was nullptr. Did you set in blueprint class?"));
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, DataTableRowName);
}

bool AWeapon::CanAttack() const
{
	const auto Character{ GetInstigator<ASaucewichCharacter>() };
	return Character && Character->Alive();
}

void AWeapon::Equip(const FWeaponData* NewWeaponData, const FName& NewDataTableRowName)
{
	WeaponData = NewWeaponData;
	DataTableRowName = NewDataTableRowName;

	// TODO: 캐릭터/무기 선택이 구현되면 비동기 로드로 변경
	Mesh->SetStaticMesh(WeaponData->Mesh.LoadSynchronous());

	static const FName SlotName{ "Color" };
	ColorDynamicMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex(SlotName)), this);
	Mesh->SetMaterialByName(SlotName, ColorDynamicMaterial);
}
