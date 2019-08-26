// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Fridge.h"

#include "GameFramework/Pawn.h"

#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"

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
