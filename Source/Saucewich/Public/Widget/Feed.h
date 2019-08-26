// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Feed.generated.h"

DECLARE_DELEGATE(FOnExpiration);

struct FFeedContent {};

UCLASS()
class SAUCEWICH_API UFeed : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeOnInitialized() override;
	
public:
	virtual void GetContent(FFeedContent& OutContent) const {}
	virtual void SetContent(const FFeedContent& InContent){}
	
	void ViewFeed();

	FOnExpiration OnExpiration;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetContent();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setting, meta = (AllowPrivateAccess = true))
	float LifeTime;

	FTimerHandle LifeTimerHandle;
	
	bool IsSet;
};
