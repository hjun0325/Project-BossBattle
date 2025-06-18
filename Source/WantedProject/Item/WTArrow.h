// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WTArrow.generated.h"

UCLASS()
class WANTEDPROJECT_API AWTArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWTArrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// OnComponentBeginOverlap 델리게이트에 의해 실행될 Overlap시 처리할 함수.
	UFUNCTION()
	void OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);
	
protected:
	UPROPERTY(EditAnywhere)
	float DamageAmount = 30.0f;
	
	// 루트 컴포넌트.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> Root;
	
	// 스태틱 메시.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> ArrowMesh;

	// 총돌 범위 설정.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> BoxComponent;

	// 발사체 설정.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;
};
