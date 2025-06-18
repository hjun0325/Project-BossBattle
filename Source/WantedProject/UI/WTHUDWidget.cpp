// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WTHUDWidget.h"

#include "WTDashWidget.h"
#include "Interface/WTCharacterHUDInterface.h"

UWTHUDWidget::UWTHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWTHUDWidget::UpdateDashCooldown(float Percent)
{
	if (Dash==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWTHUDWidget::UpdateDashCooldown"));
	}
	else
	{
		Dash->SetPercent(Percent);		
	}
}

void UWTHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Dash = Cast<UWTDashWidget>(GetWidgetFromName(TEXT("WidgetDash")));
	ensure(Dash);

	//GetOwningPlayer()->GetPawn();
	IWTCharacterHUDInterface* HUDPawn = Cast<IWTCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}
