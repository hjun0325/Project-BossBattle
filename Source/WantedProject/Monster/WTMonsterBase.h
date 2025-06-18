// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster/WTMonsterData.h"
#include "Interface/WTCharacterWidgetInterface.h"
#include "Interface/WTMonsterAIInterface.h"
#include "WTMonsterBase.generated.h"

class UWTMonsterStatComponent;
class UDataTable;

UCLASS()
class WANTEDPROJECT_API AWTMonsterBase :
public ACharacter,
public IWTCharacterWidgetInterface,
public IWTMonsterAIInterface
{
	GENERATED_BODY()

public:
	AWTMonsterBase();
	
	virtual void  PostInitializeComponents() override;
	virtual void BeginPlay() override;

	// 데미지 처리 함수.
	virtual float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	// 캐릭터 위젯 설정 함수.
	virtual void SetupCharacterWidget(class UUserWidget* InUserWidget) override;
	
	// IABCharacterAIInterface 인터페이스 함수 구현.
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	// AI가 호출할 공격 함수들.
	virtual void MeleeAttack(int32 AttackIndex) override;
	virtual void RangedAttack(int32 AttackIndex) override;
	virtual void SummonSkill() override;

	// 캐릭터에서 델리게이트를 넘길 때 사용할 함수.
	virtual void SetAIAttackDelegate(
		const FAICharacterAttackFinished& InOnAttackFinished) override;

	/*// AI가 공격을 할 때 사용할 함수.
	virtual void AttackByAI() override;*/
	
protected:
	// 죽음 상태 설정 함수.
	virtual void SetDead();

	// 죽는 애니메이션 재생 함수.
	void PlayDeadAnimation();

	// 애님 몽타주에서 제공하는 델리게이트와 파라미터 맞춤.
	void AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void NotifyComboActionEnd();
	
	//-------------------------------------------------------------------------------

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TObjectPtr<UWTMonsterStatComponent> StatComponent;
	
protected:
	// 죽은 뒤에 액터를 제거하기 전까지 대기할 시간 값.
	float DeadEventDelayTime = 5.0f;

	// UI Widget Section.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWTWidgetComponent> HpBar;
	
	// SetAIAttackDelegeate 함수로 전달된 델리게이트를 저장할 변수.
	FAICharacterAttackFinished OnAttackFinished;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UDataTable> MonsterDataTable;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FName MonsterID;

	// 데이터 테이블에서 읽어온 이 몬스터의 모든 정보를 저장할 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FMonsterData CurrentMonsterData;
};
