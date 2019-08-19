// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "TpsCharacter.h"
#include "TranslucentMatData.h"
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
			if (Owner->IsInvincible()) BeTranslucent();
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
	if (bTransl || !TranslMatData) return;

	const auto Colored = GetMesh()->GetMaterialIndex("TeamColor");
	const auto NumMat = GetMesh()->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		const auto Ptr = TranslMatData->TranslMatByIdx.Find(i);
		const auto Mat = Ptr ? *Ptr : TranslMatData->DefTranslMat;

		if (i == Colored)
		{
			const auto Color = GetColor();
			GetMesh()->CreateDynamicMaterialInstance(i, Mat)->SetVectorParameterValue("Color", Color);
		}
		else
		{
			GetMesh()->SetMaterial(i, Mat);
		}
	}

	bTransl = true;
}

void AWeapon::BeOpaque()
{
	if (!bTransl) return;

	const auto* const DefMesh = GetDefault<AWeapon>(GetClass())->GetMesh();
	const auto Colored = GetMesh()->GetMaterialIndex("TeamColor");
	const auto NumMat = GetMesh()->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		if (i == Colored)
		{
			FLinearColor Color;
			GetMesh()->GetMaterial(i)->GetVectorParameterValue({"Color"}, Color);
			Material->SetVectorParameterValue("Color", Color);
			GetMesh()->SetMaterial(i, Material);
		}
		else
		{
			GetMesh()->SetMaterial(i, DefMesh->GetMaterial(i));
		}
	}

	bTransl = false;
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
