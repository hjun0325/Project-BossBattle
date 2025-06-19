// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WTArrow.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWTArrow::AWTArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 루트 씬 컴포넌트 생성.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 스태틱 메시 컴포넌트 생성.
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->SetupAttachment(Root);
	ArrowMesh->SetCollisionProfileName(TEXT("NoCollision"));
	//ArrowMesh->SetCollisionObjectType(ECC_GameTraceChannel1);

	// 파티클 컴포넌트를 생성하고 루트에 부착.
	EffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComponent"));
	EffectComponent->SetupAttachment(RootComponent);
	
	// 박스 컴포넌트 생성.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(ArrowMesh);
	BoxComponent->SetBoxExtent(FVector(10.f, 5.f, 5.f));
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, -55.0f));
	BoxComponent->SetCollisionObjectType(ECC_GameTraceChannel3); // WTArrow.
	BoxComponent->SetCollisionProfileName(TEXT("WTArrow"));
	
	// 프로젝타일 무브먼트 컴포넌트 생성.
	ProjectileMovement =
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
	// 스태틱 메시 설정.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ArrowMeshRef(
		TEXT("/Game/ParagonSparrow/FX/Meshes/Heroes/Sparrow/Abilities/SM_Sparrow_Arrow.SM_Sparrow_Arrow"));
	if (ArrowMeshRef.Object)
	{
		ArrowMesh->SetStaticMesh(ArrowMeshRef.Object);
	}
}

// Called when the game starts or when spawned
void AWTArrow::BeginPlay()
{
	Super::BeginPlay();

	// Box 충돌 시 OnBeginOverlap함수 실행하는 델리게이트 등록.
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWTArrow::OnBeginOverlap);
}

void AWTArrow::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Monster"))
	{
		UGameplayStatics::ApplyDamage(
			OtherActor, // 피해를 받을 액터
			DamageAmount, // 피해량
			GetInstigatorController(), // 공격자의 컨트롤러
			this, // 공격을 가한 액터 (예: 화살, 총알)
			UDamageType::StaticClass() // 데미지 유형 (기본값은 UDamageType)
			);
	}
	Destroy();
}