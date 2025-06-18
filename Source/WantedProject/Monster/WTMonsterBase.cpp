// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/WTMonsterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterStat/WTMonsterStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UI/WTHpBarWidget.h"
#include "UI/WTWidgetComponent.h"
#include "AI/WTAIController.h"
#include "WTMonsterData.h"

// Sets default values
AWTMonsterBase::AWTMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 태그 설정.
	Tags.Add(FName("Monster"));

	// AIController 클래스 설정.
	AIControllerClass = AWTAIController::StaticClass();
	
	// AI 적용 대상 설정. (빙의 모드 설정)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 컴포넌트 설정.
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetCapsuleComponent()->SetCapsuleRadius(100.0f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel2); // WTMonster.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("WTMonster"));

	// 메시의 콜리전은 NoCollision 설정 (주로 랙돌에 사용됨).
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));

	// Stat Component.
	StatComponent = CreateDefaultSubobject<UWTMonsterStatComponent>(TEXT("StatComponent"));
	
	// Widget Component.
	HpBar = CreateDefaultSubobject<UWTWidgetComponent>(TEXT("Widget"));
	// 컴포넌트 계층 설정 및 상대 위치 설정 (머리 위로 보일 수 있게).
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	// 사용할 위젯 클래스 정보 설정.
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(
		TEXT("/Game/WantedProject/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		// 위젯 컴포넌트는 위젯의 클래스 정보를 바탕으로 자체적으로 인스턴스를 생성함.
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);

		// 2D 모드로 그리기.
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);

		// 크기 설정.
		HpBar->SetDrawSize(FVector2D(150.0f,15.0f));

		// 콜리전 끄기.
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWTMonsterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatComponent->OnHpZero.AddUObject(this, &AWTMonsterBase::SetDead);
}

float AWTMonsterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                 class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	StatComponent->ApplyDamage(DamageAmount);
	
	return DamageAmount;
}

void AWTMonsterBase::SetupCharacterWidget(class UUserWidget* InUserWidget)
{
	// 필요한 위젯 정보 가져오기.
	UWTHpBarWidget* HpBarWidget = Cast<UWTHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 최대 체력 값 설정.
		HpBarWidget->SetMaxHp(StatComponent->GetMaxHp());
		
		// HP 퍼센트가 제대로 계산 되도록 현재 체력 설정.
		HpBarWidget->UpdateHpBar(StatComponent->GetCurrentHp());

		// 체력 변경 이벤트(델리게이트)에 함수 및 객체 정보 등록.
		StatComponent->OnHpChanged.AddUObject(HpBarWidget, &UWTHpBarWidget::UpdateHpBar);
	}
}

void AWTMonsterBase::SetDead()
{
	// 무브먼트 컴포넌트 끄기.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	// 콜리전 끄기.
	SetActorEnableCollision(false);
	
	// 죽는 애니메이션 재생.
	PlayDeadAnimation();

	// 죽었을 때 HpBar(위젯) 사라지도록 처리.
	HpBar->SetHiddenInGame(true);
}

void AWTMonsterBase::PlayDeadAnimation()
{
	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 이미 재생 중인 몽타주가 있다면, 모두 종료.
		AnimInstance->StopAllMontages(0.0f);

		// 죽음 몽타주 재생.
		const float PlayRate = 1.0f;
		AnimInstance->Montage_Play(CurrentMonsterData.DeadMontage, PlayRate);   
	}
}

float AWTMonsterBase::GetAIPatrolRadius()
{
	return 800.0f;
}

float AWTMonsterBase::GetAIDetectRange()
{
	return 1500.0f;
}

float AWTMonsterBase::GetAIAttackRange()
{
	return 200.0f;
}

float AWTMonsterBase::GetAITurnSpeed()
{
	return 2.0f;
}

void AWTMonsterBase::MeleeAttack(int32 AttackIndex)
{
	// 이동 비활성화.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	if (!CurrentMonsterData.MeleeAttacks.IsValidIndex(AttackIndex)) return;

	// 인덱스 값의 공격 종류 설정.
	const FMeleeAttackData& AttackData = CurrentMonsterData.MeleeAttacks[AttackIndex];

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		AnimInstance->Montage_Play(AttackData.Montage, AttackSpeedRate);
	}
	
	// 몽타주 재생이 시작되면, 재생이 종료될 때 호출되는 델리게이트에 등록.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWTMonsterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData.Montage);
}

void AWTMonsterBase::RangedAttack(int32 AttackIndex)
{
	// 이동 비활성화.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	if (!CurrentMonsterData.RangedAttacks.IsValidIndex(AttackIndex)) return;

	const FRangedAttackData& AttackData = CurrentMonsterData.RangedAttacks[AttackIndex];

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		AnimInstance->Montage_Play(AttackData.Montage, AttackSpeedRate);
	}

	// 몽타주 재생이 시작되면, 재생이 종료될 때 호출되는 델리게이트에 등록.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWTMonsterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData.Montage);
}

void AWTMonsterBase::SummonSkill()
{
	// 이동 비활성화.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	const FSummonSkillData& SummonData = CurrentMonsterData.SummonSkill;

	// 소환할 병사가 유효한지 체크.
	if (!SummonData.MinionClassToSummon)
	{
		return;
	}

	// 몽타주 재생.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		const float AttackSpeedRate = 1.0f;
		AnimInstance->Montage_Play(SummonData.Montage, AttackSpeedRate);
	}

	// 몽타주 재생이 시작되면, 재생이 종료될 때 호출되는 델리게이트에 등록.
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWTMonsterBase::AttackActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SummonData.Montage);
}

void AWTMonsterBase::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AWTMonsterBase::AttackActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	// 캐릭터 무브먼트 컴포넌트 모드 복구.
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	// 공격이 끝나면 NotifyComboActionEnd 함수 호출.
	NotifyComboActionEnd();
}

void AWTMonsterBase::NotifyComboActionEnd()
{
	// 전달 받은 델리게이트 실행.
	OnAttackFinished.ExecuteIfBound();
}

void AWTMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!MonsterDataTable || MonsterID.IsNone()) return;
	
	const FMonsterData* FoundData = MonsterDataTable->FindRow<FMonsterData>(MonsterID, TEXT(""));
	if (!FoundData) return;
	CurrentMonsterData = *FoundData;

	// 1. 스탯 컴포넌트 초기화 및 죽음 이벤트 구독
	StatComponent->InitializeStat(CurrentMonsterData.StatData);
	StatComponent->OnHpZero.AddUObject(this, &AWTMonsterBase::SetDead);

	// 2. 애님 블루프린트 설정
	GetMesh()->SetAnimInstanceClass(CurrentMonsterData.AnimBlueprint);
	
	// 3. AI 비헤이비어 트리 실행
	// 현재 폰에 빙의된 컨트롤러를 우리가 만든 AWTAIController 타입으로 가져온다.
	AWTAIController* AIController = Cast<AWTAIController>(GetController());
	if (AIController)
	{
		// 데이터 테이블에서 읽어온 BehaviorTree 애셋이 유효한지 확인.
		if (CurrentMonsterData.MainBehaviorTree)
		{
			// 컨트롤러의 RunAI 함수에 해당 비헤이비어 트리를 전달하여 실행을 명령.
			AIController->RunAI(CurrentMonsterData.MainBehaviorTree);
		}
		
		UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
		UE_LOG(LogTemp, Warning, TEXT("AI Controller created"));
		if (Blackboard)
		{
			UE_LOG(LogTemp, Warning, TEXT("AI Controller created"));
			// [핵심 로직] "CombatBTAsset"이라는 키에 전투 서브 트리 애셋 정보를 저장
			Blackboard->SetValueAsObject(TEXT("CombatBTAsset"), CurrentMonsterData.CombatBehaviorTree);
		}
	}
}
