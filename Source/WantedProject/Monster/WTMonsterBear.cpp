// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/WTMonsterBear.h"
#include "AI/WTAIController.h"
#include "BrainComponent.h"
#include "MonsterStat/WTMonsterStatComponent.h"

AWTMonsterBear::AWTMonsterBear()
{
	// 스켈레탈 메시 설정.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> BearMeshRef(
		TEXT("/Game/InfinityBladeAdversaries/Enemy/Enemy_Bear/Enemy_Bear.Enemy_Bear"));
	if (BearMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(BearMeshRef.Object);
	}
}

void AWTMonsterBear::OnAcquiredFromPool_Implementation()
{
	// 액터를 보이게 하고, 충돌 및 틱을 활성화
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	// 스탯 컴포넌트의 HP를 최대치로 리셋
	if(StatComponent)
	{
		StatComponent->InitializeStat(CurrentMonsterData.StatData); // 데이터테이블 기반이라면
		// 또는 StatComponent->ResetHP(); 같은 함수를 만들어 사용
	}

	// AI를 다시 동작시킴
	AWTAIController* AIController = Cast<AWTAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StartLogic();
	}
}

void AWTMonsterBear::OnReturnedToPool_Implementation()
{
	// AI를 중지시킴
	AWTAIController* AIController = Cast<AWTAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic(FString("Returned to pool"));
	}

	// 액터를 숨기고, 충돌 및 틱을 비활성화
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void AWTMonsterBear::SetDead()
{
	Super::SetDead();

	// 자신을 비활성화.
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	
	// AI 명령 전달 중지.
	AWTAIController* WTAIController = Cast<AWTAIController>(GetController());
	if (WTAIController)
	{
		WTAIController->StopAI();
	}
	
	OnReturnedToPool_Implementation();
}
