// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UsersInfo.generated.h"

UCLASS()
class SAUCEWICH_API UUsersInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateInfo();

protected:
	void NativeOnInitialized() override;
	void NativeConstruct() override;

private:
	UPROPERTY(Transient)
	TArray<class UUserInfo*> UserInfos;
	
	class ASaucewichGameState* GameState;
};
