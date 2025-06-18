// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WTHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class WANTEDPROJECT_API UWTHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UWTHUDWidget(const FObjectInitializer& ObjectInitializer);

	// 대쉬 쿨이 업데이트될 때 사용할 함수.
	void UpdateDashCooldown(float Percent);

protected:
	virtual void NativeConstruct() override;

	// Dash 위젯.
	UPROPERTY()
	TObjectPtr<class UWTDashWidget> Dash;
};
