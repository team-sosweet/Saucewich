// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SubclassOf.h"
#include "UObject/NoExportTypes.h"
#include "CombatTextPool.generated.h"

UCLASS(Blueprintable)
class SAUCEWICH_API UCombatTextPool : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void NewCombatText(float Damage, class ATpsCharacter* DamagedActor);

private:
	void Arrange(class UCombatText* Widget);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UCombatText> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	class UUserWidget* Owner;

	UPROPERTY(Transient)
	TArray<UCombatText*> Items;
};
