// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CompositeWidget.generated.h"

UCLASS()
class SAUCEWICH_API UCompositeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeOnInitialized() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetComponent(TSubclassOf<class UComponentWidget> ComponentClass);
	
private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UComponentWidget* Component;

	UPROPERTY(Transient)
	class UCanvasPanel* RootPanel;
};
