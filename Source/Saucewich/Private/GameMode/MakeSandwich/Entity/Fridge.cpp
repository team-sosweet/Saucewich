// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Fridge.h"

#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "Online/SaucewichGameState.h"

AFridge::AFridge()
	:Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")}
{
	RootComponent = Mesh;
}

void AFridge::BeginPlay()
{
	Super::BeginPlay();
	if (IsPendingKill()) return;
	
	if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		const auto Idx = Mesh->GetMaterialIndex("TeamColor");
		const auto Mat = Mesh->CreateDynamicMaterialInstance(Idx);
		const auto& Color = GS->GetTeamData(Team).Color;
		Mat->SetVectorParameterValue("Color", Color);
	}
}

void AFridge::NotifyHit(UPrimitiveComponent* const MyComp, AActor* const Other, UPrimitiveComponent* const OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (IsNetMode(NM_Client)) return;
	
	const auto Pawn = Cast<APawn>(Other);
	if (!Pawn) return;

	const auto Player = Pawn->GetPlayerState<AMakeSandwichPlayerState>();
	if (!Player) return;
	if (Player->GetTeam() != Team) return;

	Player->PutIngredientsInFridge();
}
