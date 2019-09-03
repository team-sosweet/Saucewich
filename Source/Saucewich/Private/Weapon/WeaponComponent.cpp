// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "WeaponComponent.h"

#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "Player/TpsCharacter.h"
#include "Weapon/Gun.h"
#include "SaucewichGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, Log, All)

UWeaponComponent::UWeaponComponent()
{
	bReplicates = true;
	bWantsInitializeComponent = true;
}

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const_cast<ATpsCharacter*&>(Owner) = CastChecked<ATpsCharacter>(GetOwner());
	Weapons.Init(nullptr, WeaponSlots);
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

	for (auto Wep : Weapons)
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

void UWeaponComponent::BeOpaque()
{
	for (const auto Wep : Weapons)
		if (Wep) Wep->BeOpaque();
}

void UWeaponComponent::BeTranslucent()
{
	for (const auto Wep : Weapons)
		if (Wep) Wep->BeTranslucent();
}

float UWeaponComponent::GetSpeedRatio() const
{
	if (const auto Weapon = GetActiveWeapon())
		if (const auto Data = Weapon->GetData(FILE_LINE_FUNC))
			return Data->WalkSpeedRatio;
	return 1.f;
}

void UWeaponComponent::OnCharacterDeath()
{
	for (auto& Weapon : Weapons)
	{
		if (Weapon)
		{
			Weapon->Release();
		}
	}
}

void UWeaponComponent::SetColor(const FLinearColor& NewColor)
{
	for (const auto Weapon : Weapons)
	{
		if (Weapon)
		{
			UE_LOG(LogWeaponComponent, Log, TEXT("Set color of %s from %s to %s"), *Weapon->GetName(), *Weapon->GetColor().ToString(), *NewColor.ToString());
			Weapon->SetColor(NewColor);
		}
	}
}

AWeapon* UWeaponComponent::Give(const TSubclassOf<AWeapon> WeaponClass)
{
	if (!WeaponClass) return nullptr;

	const auto Data = GetDefault<AWeapon>(WeaponClass)->GetData(FILE_LINE_FUNC);
	if (!Data) return nullptr;
	
	const auto Slot = Data->Slot;
	if (Slot >= Weapons.Num())
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Failed to give weapon: Invalid slot (Expected: < %d, Actual: %d)"), Weapons.Num(), Slot);
		return nullptr;
	}

	const auto GI = GetWorld()->GetGameInstance<USaucewichGameInstance>();
	if (!GI)
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Give: USaucewichGameInstance를 얻는 데 실패했습니다."));
		return nullptr;
	}

	const auto Pool = GI->GetActorPool();
	if (!Pool)
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Give: ActorPool을 얻는 데 실패했습니다."));
		return nullptr;
	}

	if (Weapons[Slot])
	{
		Weapons[Slot]->Release();
		if (Weapons[Slot]->GetClass() == WeaponClass)
		{
			Weapons[Slot]->Activate();
			return Weapons[Slot];
		}
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = Owner;
	Parameters.Instigator = Owner;

	const auto Weapon = Pool->Spawn<AWeapon>(WeaponClass, FTransform::Identity, Parameters);
	if (!Weapon)
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Failed to give weapon: Can't spawn the weapon"));
		return nullptr;
	}

	Weapon->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	if (Slot == 0 && !Weapons[Slot]) Owner->SetMaxHP(Data->HPRatio);
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
}

void UWeaponComponent::FireP()
{
	if (!Owner->IsAlive()) return;
	if (auto W = GetActiveWeapon())
	{
		W->FireP();
		if (Owner->IsLocallyControlled()) ServerFireP();
	}
}

void UWeaponComponent::ServerFireP_Implementation() { MulticastFireP(); }
bool UWeaponComponent::ServerFireP_Validate() { return true; }
void UWeaponComponent::MulticastFireP_Implementation() { if (!Owner->IsLocallyControlled()) FireP(); }

void UWeaponComponent::FireR()
{
	if (!Owner->IsAlive()) return;
	if (auto W = GetActiveWeapon())
	{
		W->FireR();
		if (Owner->IsLocallyControlled()) ServerFireR();
	}
}

void UWeaponComponent::ServerFireR_Implementation() { MulticastFireR(); }
bool UWeaponComponent::ServerFireR_Validate() { return true; }
void UWeaponComponent::MulticastFireR_Implementation() { if (!Owner->IsLocallyControlled()) FireR(); }

void UWeaponComponent::SlotP(const uint8 Slot)
{
	if (!Owner->IsAlive()) return;
	if (auto W = Weapons[Slot])
	{
		W->SlotP();
		if (Owner->IsLocallyControlled()) ServerSlotP(Slot);
	}
}

void UWeaponComponent::ServerSlotP_Implementation(const uint8 Slot) { MulticastSlotP(Slot); }
bool UWeaponComponent::ServerSlotP_Validate(const uint8 Slot) { return Slot < Weapons.Num(); }
void UWeaponComponent::MulticastSlotP_Implementation(const uint8 Slot) { if (!Owner->IsLocallyControlled()) SlotP(Slot); }

void UWeaponComponent::SlotR(const uint8 Slot)
{
	if (!Owner->IsAlive()) return;
	if (auto W = Weapons[Slot])
	{
		W->SlotR();
		if (Owner->IsLocallyControlled()) ServerSlotR(Slot);
	}
}

void UWeaponComponent::ServerSlotR_Implementation(const uint8 Slot) { MulticastSlotR(Slot); }
bool UWeaponComponent::ServerSlotR_Validate(const uint8 Slot) { return Slot < Weapons.Num(); }
void UWeaponComponent::MulticastSlotR_Implementation(const uint8 Slot) { if (!Owner->IsLocallyControlled()) SlotR(Slot); }
