// Copyright 2019 Othereum. All Rights Reserved.

#include "Fridge.h"

#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "MakeSandwichPlayerState.h"
#include "SaucewichGameMode.h"

AFridge::AFridge()
	:Mesh{CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"))}
{
	RootComponent = Mesh;
}

void AFridge::BeginPlay()
{
	Super::BeginPlay();
	
	const auto Idx = Mesh->GetMaterialIndex(TEXT("TeamColor"));
	const auto Mat = Mesh->CreateDynamicMaterialInstance(Idx);
	auto&& Color = ASaucewichGameMode::GetData(this).Teams[Team].Color;
	Mat->SetVectorParameterValue(TEXT("Color"), Color);
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
