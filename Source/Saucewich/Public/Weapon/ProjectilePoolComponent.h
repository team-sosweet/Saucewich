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
	class AProjectile* Spawn(const FRotator& Rotation);
	class AProjectile* Spawn(const FQuat& Rotation);
	AProjectile* Spawn(const FTransform& Transform);
	void Release(AProjectile* Projectile);
	TSubclassOf<AProjectile> GetProjectileClass() const { return Class; }

protected:
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<AProjectile> Class;

	UPROPERTY(Transient, VisibleInstanceOnly)
	TArray<AProjectile*> Pool;
};
