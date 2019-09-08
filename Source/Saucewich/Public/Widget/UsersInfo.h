// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UsersInfo.generated.h"

UCLASS()
class SAUCEWICH_API UUsersInfo final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Info"))
	void UpdateInfo();
	
private:
	UPROPERTY(Transient)
	TArray<class UUserInfo*> UserInfos;
	
	class ASaucewichGameState* GameState;
};
