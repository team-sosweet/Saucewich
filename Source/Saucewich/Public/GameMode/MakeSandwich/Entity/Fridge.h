// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameMode/MakeSandwich/MakeSandwichPlayerState.h"
#include "Fridge.generated.h"

UCLASS()
class SAUCEWICH_API AFridge : public AActor
{
	GENERATED_BODY()

public:
	AFridge();
	uint8 GetTeam() const { return Team; }

protected:
	void BeginPlay() override;
	
	void Tick(float DeltaSeconds) override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
private:
	class UFridgeHUD* GetHUD() const;
	void BindPS(class ASaucewichPlayerState* InPS);
	void OnPlyTeamChanged(uint8 NewTeam);
	void OnIngChanged(class AMakeSandwichPlayerState* InPS) const;
	void SetHighlighted(bool bHighlight) const;
	
	AMakeSandwichPlayerState* LocalPS;
	FDelegateHandle OnIngChangedHandle;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HUD;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
