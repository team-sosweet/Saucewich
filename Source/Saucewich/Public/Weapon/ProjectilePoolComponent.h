// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProjectilePoolComponent.generated.h"

UCLASS()
class UProjectilePoolComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	void Release(class AProjectile* Projectile);
	AProjectile* Spawn(const FQuat& Rotation, bool bCosmetic);
	AProjectile* Spawn(const FTransform& Transform, bool bCosmetic);
	TSubclassOf<AProjectile> GetProjectileClass() const { return Class; }

protected:
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<AProjectile> Class;
	TArray<AProjectile*> Pool;
};
