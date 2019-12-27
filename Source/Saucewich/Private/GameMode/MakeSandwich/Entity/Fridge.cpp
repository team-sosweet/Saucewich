// Copyright 2019 Othereum. All Rights Reserved.

#include "GameMode/MakeSandwich/Entity/Fridge.h"

#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Player/SaucewichPlayerController.h"
#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "GameMode/SaucewichGameMode.h"
#include "Names.h"
#include "Widget/FridgeHUD.h"

AFridge::AFridge()
	:Mesh{CreateDefaultSubobject<UStaticMeshComponent>(Names::Mesh)}
{
	RootComponent = Mesh;

#if !UE_SERVER
	HUD = CreateDefaultSubobject<UWidgetComponent>(NAME("HUD"));
	HUD->SetupAttachment(Mesh);

	PrimaryActorTick.bCanEverTick = true;
#endif
}

void AFridge::BeginPlay()
{
	Super::BeginPlay();
	
	const auto Idx = Mesh->GetMaterialIndex(Names::TeamColor);
	const auto Mat = Mesh->CreateDynamicMaterialInstance(Idx);
	auto&& Color = ASaucewichGameMode::GetData(this).Teams[Team].Color;
	Mat->SetVectorParameterValue(Names::Color, Color);

#if !UE_SERVER
	if (!IsNetMode(NM_DedicatedServer))
	{
		GetHUD()->Init(Team);
		
		const auto PC = CastChecked<ASaucewichPlayerController>(GetWorld()->GetFirstPlayerController());
		PC->SafePS(FOnPSSpawnedNative::FDelegate::CreateUObject(this, &AFridge::BindPS));
	}
#endif 
}

void AFridge::NotifyHit(UPrimitiveComponent* const MyComp, AActor* const Other, UPrimitiveComponent* const OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (IsNetMode(NM_Client)) return;
	
	const auto Pawn = Cast<APawn>(Other);
	if (!Pawn) return;

	const auto Player = Pawn->GetPlayerStateChecked<AMakeSandwichPlayerState>();
	if (Player->GetTeam() != Team) return;

	Player->PutIngredientsInFridge();
}


#if !UE_SERVER

void AFridge::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsNetMode(NM_DedicatedServer))
	{
		if (const auto Pawn = GetWorld()->GetFirstPlayerController()->GetPawn())
		{
			const auto Dist = FVector::Dist(GetActorLocation(), Pawn->GetActorLocation());
			const auto Size = FMath::GetMappedRangeValueClamped({0, 2000}, {100, 40}, Dist);
			HUD->SetDrawSize({Size, Size});
		}
	}
}

UFridgeHUD* AFridge::GetHUD() const
{
	return CastChecked<UFridgeHUD>(HUD->GetUserWidgetObject());
}

void AFridge::BindPS(ASaucewichPlayerState* const InPS)
{
	LocalPS = CastChecked<AMakeSandwichPlayerState>(InPS);
	InPS->BindOnTeamChanged(FOnTeamChangedNative::FDelegate::CreateUObject(this, &AFridge::OnPlyTeamChanged));
}

void AFridge::OnPlyTeamChanged(const uint8 NewTeam)
{
	if (NewTeam == Team)
	{
		OnIngChangedHandle = LocalPS->OnIngChangedNative.AddUObject(this, &AFridge::OnIngChanged);
	}
	else
	{
		SetHighlighted(false);
		LocalPS->OnIngChangedNative.Remove(OnIngChangedHandle);
	}
}

void AFridge::OnIngChanged(AMakeSandwichPlayerState* const InPS) const
{
	SetHighlighted(InPS->GetNumIngredients() > 0);
}

void AFridge::SetHighlighted(const bool bHighlight) const
{
	GetHUD()->SetHighlighted(bHighlight);
	Mesh->SetRenderCustomDepth(bHighlight);
}


#endif
