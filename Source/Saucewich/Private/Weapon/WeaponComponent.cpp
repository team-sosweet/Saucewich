// Copyright 2019 Othereum. All Rights Reserved.

#include "Weapon/WeaponComponent.h"

#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "Player/TpsCharacter.h"
#include "Weapon/Gun.h"
#include "UserSettings.h"

UWeaponComponent::UWeaponComponent()
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::AddOnEquipWeapon(const FOnEquipWeaponSingle& Delegate, const bool bCallbackWithCurrentWeapons)
{
	if (!Delegate.IsBound()) return;
	if (bCallbackWithCurrentWeapons)
		for (const auto Weapon : Weapons)
			if (Weapon) Delegate.Execute(Weapon);
	OnEquipWeapon.AddUnique(Delegate);
}

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Weapons.Init(nullptr, WeaponSlots);
}

void UWeaponComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* const ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto Owner = Cast<APawn>(GetOwner());
	if (Owner && Owner->IsLocallyControlled())
	{
		if (UUserSettings::Get()->bAutoFire)
		{
			FHitResult Hit;
			bShouldAutoFire = GunTrace(Hit) == EGunTraceHit::Pawn;
		}
		else
		{
			bShouldAutoFire = false;
		}

		const auto bShouldFire = bShouldAutoFire || bFirePressed;
		if (bFiring)
		{
			if (!bShouldFire) StopFire();
		}
		else
		{
			if (bShouldFire) StartFire();
		}
	}
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, Weapons);
	DOREPLIFETIME(UWeaponComponent, Active);
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (auto& Wep : Weapons)
	{
		if (Wep) 
		{
			Wep->Release();
			Wep = nullptr;
		}
	}
}

void UWeaponComponent::SetupPlayerInputComponent(UInputComponent* Input)
{
	Input->BindAction("Fire", IE_Pressed, this, &UWeaponComponent::FireP);
	Input->BindAction("Fire", IE_Released, this, &UWeaponComponent::FireR);

	FString Slot = TEXT("Slot0");
	for (uint8 i = 0; i < WeaponSlots; ++i)
	{
		using F = TBaseDelegate<void, uint8>;
		Slot[4] = TEXT('1') + i;
		Input->BindAction<F>(*Slot, IE_Pressed, this, &UWeaponComponent::SlotP, i);
		Input->BindAction<F>(*Slot, IE_Released, this, &UWeaponComponent::SlotR, i);
	}
}

EGunTraceHit UWeaponComponent::GunTrace(FHitResult& OutHit) const
{
	if (const auto Gun = Cast<AGun>(GetActiveWeapon()))
		return Gun->GunTrace(OutHit);
	return EGunTraceHit::None;
}

bool UWeaponComponent::TrySelectWeapon(const uint8 Slot)
{
	if (Slot >= WeaponSlots) return false;

	const auto DesiredWep = Weapons[Slot];
	if (!DesiredWep || !DesiredWep->CanDeploy()) return false;

	const auto ActiveWep = GetActiveWeapon();
	if (ActiveWep && !ActiveWep->CanHolster()) return false;

	ActiveWep->Holster();
	DesiredWep->Deploy();
	Active = Slot;
	return true;
}

float UWeaponComponent::GetSpeedRatio() const
{
	if (const auto Weapon = GetActiveWeapon())
		return Weapon->GetWeaponData().WalkSpeedRatio;
	return 1;
}

float UWeaponComponent::GetArmorRatio() const
{
	if (const auto Weapon = GetWeapon(0))
		return Weapon->GetWeaponData().ArmorRatio;
	return 1;
}

void UWeaponComponent::OnCharacterDeath()
{
	if (GetOwner()->HasAuthority())
	{
		for (auto& Weapon : Weapons)
		{
			if (Weapon)
			{
				Weapon->Release();
				Weapon = nullptr;
			}
		}
	}
}

void UWeaponComponent::SetColor(const FLinearColor& NewColor)
{
	for (const auto Weapon : Weapons)
		if (Weapon) Weapon->SetColor(NewColor);
}

AWeapon* UWeaponComponent::Give(const TSubclassOf<AWeapon> WeaponClass)
{
	if (!ensure(WeaponClass)) return nullptr;

	const auto Slot = GetDefault<AWeapon>(WeaponClass)->GetData().Slot;
	if (!ensure(Slot < Weapons.Num())) return nullptr;

	if (Weapons[Slot])
	{
		Weapons[Slot]->Release();
		Weapons[Slot] = nullptr;
	}

	FActorSpawnParameters Parameters;
	Parameters.Instigator = CastChecked<ATpsCharacter>(GetOwner());
	Parameters.Owner = Parameters.Instigator;

	const auto Weapon = AActorPool::Get(this)->Spawn<AWeapon>(WeaponClass, FTransform::Identity, Parameters);
	if (!ensure(Weapon)) return nullptr;

	Weapon->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	Weapons[Slot] = Weapon;
	if (Slot == Active) Weapon->Deploy();

	return Weapon;
}

void UWeaponComponent::OnRep_Weapons()
{
	const auto ActiveWeapon = GetActiveWeapon();
	if (ActiveWeapon && !ActiveWeapon->IsVisible())
	{
		ActiveWeapon->SetVisibility(true);
	}

	for (const auto Wep : Weapons)
		if (Wep) Wep->SetColor(CastChecked<ATpsCharacter>(GetOwner())->GetColor());
}

void UWeaponComponent::FireP()
{
	bFirePressed = true;
}

void UWeaponComponent::ServerFireP_Implementation() { MulticastFireP(); }
bool UWeaponComponent::ServerFireP_Validate() { return true; }
void UWeaponComponent::MulticastFireP_Implementation()
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (Owner && !Owner->IsLocallyControlled()) StartFire();
}

void UWeaponComponent::FireR()
{
	bFirePressed = false;
}

void UWeaponComponent::StartFire()
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (!Owner || !Owner->IsAlive()) return;
	if (auto W = GetActiveWeapon())
	{
		W->FireP();
		if (Owner->IsLocallyControlled()) ServerFireP();
	}
	bFiring = true;
}

void UWeaponComponent::StopFire()
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (!Owner || !Owner->IsAlive()) return;
	if (auto W = GetActiveWeapon())
	{
		W->FireR();
		if (Owner->IsLocallyControlled()) ServerFireR();
	}
	bFiring = false;
}

void UWeaponComponent::ServerFireR_Implementation() { MulticastFireR(); }
bool UWeaponComponent::ServerFireR_Validate() { return true; }
void UWeaponComponent::MulticastFireR_Implementation()
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (Owner && !Owner->IsLocallyControlled()) StopFire();
}

void UWeaponComponent::SlotP(const uint8 Slot)
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (!Owner || !Owner->IsAlive()) return;
	if (auto W = Weapons[Slot])
	{
		W->SlotP();
		if (Owner->IsLocallyControlled()) ServerSlotP(Slot);
	}
}

void UWeaponComponent::ServerSlotP_Implementation(const uint8 Slot) { MulticastSlotP(Slot); }
bool UWeaponComponent::ServerSlotP_Validate(const uint8 Slot) { return Slot < Weapons.Num(); }
void UWeaponComponent::MulticastSlotP_Implementation(const uint8 Slot)
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (Owner && !Owner->IsLocallyControlled()) SlotP(Slot);
}

void UWeaponComponent::SlotR(const uint8 Slot)
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (!Owner || !Owner->IsAlive()) return;
	if (auto W = Weapons[Slot])
	{
		W->SlotR();
		if (Owner->IsLocallyControlled()) ServerSlotR(Slot);
	}
}

void UWeaponComponent::ServerSlotR_Implementation(const uint8 Slot) { MulticastSlotR(Slot); }
bool UWeaponComponent::ServerSlotR_Validate(const uint8 Slot) { return Slot < Weapons.Num(); }
void UWeaponComponent::MulticastSlotR_Implementation(const uint8 Slot)
{
	const auto Owner = Cast<ATpsCharacter>(GetOwner());
	if (Owner && !Owner->IsLocallyControlled()) SlotR(Slot);
}
