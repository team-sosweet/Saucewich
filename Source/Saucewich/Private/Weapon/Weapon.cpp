// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "Player/TpsCharacter.h"
#include "Weapon/WeaponComponent.h"
#include "Weapon/WeaponSharedData.h"

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
			SetColor(Character->GetColor());
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

int32 AWeapon::GetColIdx() const
{
	return GUARANTEE(SharedData != nullptr) ? Mesh->GetMaterialIndex(SharedData->ColMatName) : INDEX_NONE;
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GUARANTEE(SharedData != nullptr))
	{
		if (const auto Data = GetData(FILE_LINE_FUNC))
		{
			const auto ColMatIdx = GetColIdx();
			if (GUARANTEE(ColMatIdx != INDEX_NONE))
			{
				const auto Transl = SharedData->GetTranslMat(GetMesh()->GetMaterial(ColMatIdx));
				if (GUARANTEE(Transl != nullptr))
				{
					ColTranslMat = UMaterialInstanceDynamic::Create(Transl, GetMesh());
				}
				ColMat = GetMesh()->CreateDynamicMaterialInstance(ColMatIdx);

				OnColMatCreated.Broadcast();
			}
		}
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
	if (ColMat) ColMat->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

void AWeapon::SetColor(const FLinearColor& NewColor)
{
	if (ColMat) ColMat->SetVectorParameterValue("Color", NewColor);
	if (ColTranslMat) ColTranslMat->SetVectorParameterValue("Color", NewColor);
}

void AWeapon::BeTranslucent()
{
	if (bTransl) return;
	if (!GUARANTEE(SharedData != nullptr)) return;

	const auto Data = GetData(FILE_LINE_FUNC);
	if (!Data) return;

	const auto ColMatIdx = GetColIdx();
	const auto NumMat = GetMesh()->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		if (i == ColMatIdx)
			GetMesh()->SetMaterial(i, ColTranslMat);
		
		else if (const auto Transl = SharedData->GetTranslMat(GetMesh()->GetMaterial(i)))
			GetMesh()->SetMaterial(i, Transl);
	}

	bTransl = true;
}

void AWeapon::BeOpaque()
{
	if (!bTransl) return;

	const auto* const DefMesh = GetDefault<AWeapon>(GetClass())->Mesh;
	const auto NumMat = Mesh->GetNumMaterials();
	const auto ColMatIdx = GetColIdx();
	
	for (auto i = 0; i < NumMat; ++i)
		GetMesh()->SetMaterial(i, i == ColMatIdx ? ColMat : DefMesh->GetMaterial(i));

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
