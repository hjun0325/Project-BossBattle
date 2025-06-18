// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ObjectPoolManager.h"
#include "Interface/WTPoolableObjectInterface.h"

AObjectPoolManager::AObjectPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("ObjectPoolManager: Initializing pool..."));

	// 게임이 시작될 때, PoolableClasses 맵에 지정된 정보대로 오브젝트들을 미리 생성.
	for (auto const& [ObjectClass, PoolSize] : PoolableClasses)
	{
		if (ObjectClass)
		{
			UE_LOG(LogTemp, Log, TEXT("Found Class '%s' to pool. Size: %d"), *ObjectClass->GetName(), PoolSize);
			
			FObjectPool NewPool;
			for (int32 i = 0; i < PoolSize; ++i)
			{
				// 1. 스폰 파라미터를 설정합니다.
				FActorSpawnParameters SpawnParams;
				// 2. 충돌 처리 방법을 '충돌을 무시하고 항상 스폰'으로 변경합니다.
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				// 월드에 액터를 스폰.
				AActor* NewObj = GetWorld()->SpawnActor<AActor>(ObjectClass, GetActorLocation(), FRotator::ZeroRotator,SpawnParams);
				if (NewObj)
				{
					UE_LOG(LogTemp, Log, TEXT("  - Spawned %s successfully."), *NewObj->GetName());
					// 스폰 직후 즉시 비활성화시켜서 '재고' 생성.
					NewObj->SetActorHiddenInGame(true);
					NewObj->SetActorEnableCollision(false);
					NewObj->SetActorTickEnabled(false);
					NewPool.PooledObjects.Add(NewObj);
				}
			}
			PoolMap.Add(ObjectClass, NewPool);
		}
	}
}

AActor* AObjectPoolManager::AcquirePooledObject(TSubclassOf<AActor> ObjectClass)
{
	// 1. [] 연산자 대신 Find 함수를 사용하여 해당 클래스의 풀에 대한 포인터를 가져옵니다.
	FObjectPool* Pool = PoolMap.Find(ObjectClass);

	// 2. 만약 풀이 존재한다면,
	if (Pool)
	{
		// 해당 풀에 있는 모든 오브젝트를 하나씩 확인합니다.
		for (AActor* Obj : Pool->PooledObjects)
		{
			// 오브젝트가 유효하고, 틱이 꺼져있는지(비활성 상태인지) 확인합니다.
			if (Obj && !Obj->IsActorTickEnabled()) 
			{
				// --- 여기가 가장 중요한 수정 부분 ---
				// 3. 찾았으면, 반환하기 직전에 '활성화' 함수를 호출하여 깨웁니다.
				if (Obj->GetClass()->ImplementsInterface(UWTPoolableObjectInterface::StaticClass()))
				{
					IWTPoolableObjectInterface::Execute_OnAcquiredFromPool(Obj);
				}
                
				// 4. 깨어난 오브젝트를 요청한 곳에 전달합니다.
				return Obj;
			}
		}
	}
    
	// 만약 루프를 다 돌았는데도 사용 가능한 오브젝트가 없다면(모두 사용 중이라면),
	// 경고 로그를 남기고 없다고 알립니다.
	UE_LOG(LogTemp, Warning, TEXT("No available object in pool for class %s"), *ObjectClass->GetName());
	return nullptr;
}
