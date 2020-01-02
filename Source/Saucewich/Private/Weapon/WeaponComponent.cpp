// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Weapon/WeaponComponent.h"

#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "Player/TpsCharacter.h"
#include "Weapon/Gun.h"
#include "UserSettings.h"
#include "Names.h"

UWeaponComponent::UWeaponComponent()
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::AddOnEquipWeapon(const FOnEquipWeaponSingle& Delegate)
{
	check(Delegate.IsBound());

	for (const auto Weapon : Weapons)
		if (Weapon) Delegate.Execute(Weapon);

	check(!OnEquipWeapon.Contains(Delegate));
	OnEquipWeapon.Add(Delegate);
}

void UWeaponComponent::AddOnWepAvailabilityChanged(const uint8 Slot, const FOnWepAvailabilityChangedSingle& Delegate)
{
	OnWepAvailabilityChanged[Slot].Add(Delegate);
}

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	Weapons.AddZeroed(WeaponSlots);
	OnWepAvailabilityChanged.AddDefaulted(WeaponSlots);
}

void UWeaponComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* const ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto Owner = CastChecked<APawn>(GetOwner());
	if (Owner->IsLocallyControlled())
	{
		if (UUserSettings::Get(this)->bAutoFire)
		{
			FHitResult Hit;
			bShouldAutoFire = GunTrace(Hit);
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
	ClearWeapons();
}

void UWeaponComponent::SetupPlayerInputComponent(UInputComponent* Input)
{
	Input->BindAction(NAME("Fire"), IE_Pressed, this, &UWeaponComponent::FireP);
	Input->BindAction(NAME("Fire"), IE_Released, this, &UWeaponComponent::FireR);

	FString Slot = TEXT("Slot0");
	for (uint8 i = 0; i < WeaponSlots; ++i)
	{
		Slot[4] = TEXT('1') + i;
		using F = TBaseDelegate<void, uint8>;
		Input->BindAction<F>(*Slot, IE_Pressed, this, &UWeaponComponent::SlotP, i);
		Input->BindAction<F>(*Slot, IE_Released, this, &UWeaponComponent::SlotR, i);
	}
}

bool UWeaponComponent::GunTrace(FHitResult& OutHit) const
{
	if (const auto Gun = Cast<AGun>(GetActiveWeapon()))
		return Gun->GunTrace(OutHit);
	
	return false;
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
		ClearWeapons();
	}
}

void UWeaponComponent::SetColor(const FLinearColor& NewColor)
{
	for (const auto Weapon : Weapons)
		if (Weapon) Weapon->SetColor(NewColor);
}

AWeapon* UWeaponComponent::Give(const TSoftClassPtr<AWeapon>& WeaponClass)
{
	const auto Cls = WeaponClass.LoadSynchronous();
	check(Cls);

	const auto Slot = GetDefault<AWeapon>(Cls)->GetData().Slot;
	check(Slot < Weapons.Num());

	FActorSpawnParameters Parameters;
	Parameters.Instigator = CastChecked<ATpsCharacter>(GetOwner());
	Parameters.Owner = Parameters.Instigator;

	const auto Weapon = AActorPool::Get(this)->Spawn<AWeapon>(Cls, FTransform::Identity, Parameters);
	check(Weapon);

	Weapon->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	if (Weapons[Slot]) Weapons[Slot]->Release();
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

	const auto Color = CastChecked<ATpsCharacter>(GetOwner())->GetColor();
	for (const auto Wep : Weapons)
		if (Wep) Wep->SetColor(Color);
}

void UWeaponComponent::ClearWeapons()
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

UWeaponComponent::FBroadcastAvailabilityChanged::FBroadcastAvailabilityChanged(
	UWeaponComponent* Comp, const AWeapon* Wep, const bool bAvailable)
{
	Comp->OnWepAvailabilityChanged[Wep->GetData().Slot].Broadcast(bAvailable);
}

void UWeaponComponent::FireP()
{
	bFirePressed = true;
}

void UWeaponComponent::ServerFireP_Implementation() { MulticastFireP(); }
bool UWeaponComponent::ServerFireP_Validate() { return true; }
void UWeaponComponent::MulticastFireP_Implementation()
{
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsLocallyControlled()) StartFire();
}

void UWeaponComponent::FireR()
{
	bFirePressed = false;
}

void UWeaponComponent::StartFire()
{
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsAlive()) return;
	if (auto W = GetActiveWeapon())
	{
		W->FireP();
		if (Owner->IsLocallyControlled()) ServerFireP();
	}
	bFiring = true;
}

void UWeaponComponent::StopFire()
{
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsAlive()) return;
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
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsLocallyControlled()) StopFire();
}

void UWeaponComponent::SlotP(const uint8 Slot)
{
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsAlive()) return;
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
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsLocallyControlled()) SlotP(Slot);
}

void UWeaponComponent::SlotR(const uint8 Slot)
{
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsAlive()) return;
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
	const auto Owner = CastChecked<ATpsCharacter>(GetOwner());
	if (!Owner->IsLocallyControlled()) SlotR(Slot);
}
