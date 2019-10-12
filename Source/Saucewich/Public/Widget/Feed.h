// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Feed.generated.h"

DECLARE_DELEGATE(FOnExpiration);

USTRUCT()
struct FFeedContent
{
	GENERATED_BODY()
};

UCLASS()
class SAUCEWICH_API UFeed : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeOnInitialized() override;
	
public:
	virtual void GetContent(FFeedContent& OutContent) const {}
	virtual void SetContent(const FFeedContent& InContent) { OnSetContent(); }
	
	void ViewFeed(float LifeTime);

	float GetCurLifeTime() const;
	
	FOnExpiration OnExpiration;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetContent();
	
private:
	void LifetimeExpired();
	
	FTimerHandle LifeTimerHandle;
};
