// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon.h"

#include "Components/StaticMeshComponent.h"
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

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Mesh->SetVisibility(false);
	Init();
}

void AWeapon::Init()
{
	if (const auto Character = Cast<ATpsCharacter>(GetOwner()))
	{
		Owner = Character;
		Role = Owner->Role;
		if (Owner->GetWeaponComponent()->GetActiveWeapon() == this)
		{
			Deploy();
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

void AWeapon::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (const auto Character = GetCharacter())
	{
		Role = Character->Role;
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, bEquipped);
}

bool AWeapon::IsVisible() const
{
	return Mesh->bVisible;
}

void AWeapon::SetVisibility(const bool bNewVisibility) const
{
	Mesh->SetVisibility(bNewVisibility);
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
