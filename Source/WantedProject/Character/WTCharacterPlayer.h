// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/WTCharacterBase.h"
#include "Interface/WTAttackAnimationInterface.h"
#include "Interface/WTCharacterWidgetInterface.h"
#include "Interface/WTCharacterHUDInterface.h"
#include "WTCharacterPlayer.generated.h"

/**
 * 
 */

UENUM()
enum class EUltimateSkillState : uint8
{
	None,
	Charging,
	Firing
};


/** Forward declaration to improve compiling times */
class AWTArrow;
class UAnimMontage;
class UInputAction;
class UNiagaraSystem;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class WANTEDPROJECT_API AWTCharacterPlayer :
public AWTCharacterBase,
public IWTAttackAnimationInterface,
public IWTCharacterWidgetInterface,
public IWTCharacterHUDInterface
{
	GENERATED_BODY()

public:
	AWTCharacterPlayer();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	// 기능을 입력에 바인딩하기 위한 함수.
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 컨트롤 타입에 변경에 따라 추가적으로 설정해야 하는 함수. 
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);

	// 캐릭터 컨트롤 데이터 값으로 설정하는 함수.
	virtual void SetCharacterControlData(const UWTCharacterControlData* InCharacterControlData) override;
	
protected:
	// V키에 바인딩할 함수.
	// 사용할 캐릭터 컨트롤을 변경하는 함수.
	void ChangeCharacterController();

	// Dash 함수.
	void Dash();
	void ResetDashCooldown();

	// TPS 입력.
	void TPSMove(const struct FInputActionValue& Value);
	void TPSLook(const struct FInputActionValue& Value);

	// TopDown 입력.
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();

	// Shot 함수.
	void TopDownShot();
	void UltimateSkillStarted();
	void UltimateSkillCharging();
	void UltimateSkillFiring();

	// LineTrace 함수.
	virtual void LineTraceSingle() override;
	virtual void LineTraceMulti() override;

	// 어택 가능한지 설정하는 함수.
	virtual void SetAttacking(bool isAttacking) override;

	// 화살 발사 하는 함수.
	virtual void FireArrow() override;

	// 캐릭터 위젯 설정 함수.
	virtual void SetupCharacterWidget(class UUserWidget* InUserWidget) override;

	// IABCharacterHUDInterface 함수 구현.
	virtual void SetupHUDWidget(class UWTHUDWidget* InHUDWidget) override;

	// 궁극기 스킬 화살 발사하는 함수.
	virtual void UlitmateSkillArrow() override;

	// 몬스터의 공격을 '수신'
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 죽음 상태 설정 함수.
	virtual void SetDead();

	// 죽는 애니메이션 재생 함수.
	void PlayDeadAnimation();
	
protected:
	// Camera Section.
	// Meta는 Private로 선언된 언리얼 오브젝트 객체들을 블루프린트에서도 접근할 수 있도록 만들어주는 특별한 지시자.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	// Input Section.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeControlAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TPSLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TPSMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TopDownClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TopDownShotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> UltimateSkillAction;
	
	// 현재 사용 중인 캐릭터 모드.
	ECharacterControlType CurrentCharacterControlType;

	EUltimateSkillState CurrentSkillState;

	// 죽음 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeadMontage;
	
	// 공격 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AttackMontage;

	// 궁극기 몽타주 애셋.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> UltimateSkillMontage;

	// 화살.
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWTArrow> ArrowClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;

	// Effect Section.
	// 클릭 시 생성할 FX 이펙트 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;
	
	// 나이아가라 시스템 화살 발사.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* NSArrowCast;

	// 화살 발사 사운드.
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ArrowCastCue;
	
	// 나이아가라 시스템 대쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* NSDash;

	// 대쉬 사운드.
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* DashSoundCue;

	// 짧은 클릭으로 인식할 시간 문턱 값.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;
	
	// trace 길이.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	float TraceDistance = 3000.0f;

	// 디버그 유지 시간.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	float DrawDebugLifeTime = 20.0f;

	// Stat Section.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWTCharacterStatComponent> Stat;

	// UI Widget Section.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWTWidgetComponent> HpBar;
	
	// 변경 사항을 전달하기 위한 HUD위젯 선언.
	UPROPERTY()
	TObjectPtr<class UWTHUDWidget> HUDWidget;
	
private:
	// 목적지 위치.
	FVector CachedDestination;

	bool bIsPreparingToAttack = true;

	// 터치 디바이스인지 여부 (1비트 플래그).
	uint8 bIsTouch : 1;

	// 클릭(또는 터치)이 얼마나 오래 지속되었는지 시간.
	float FollowTime;

	// 공격 체크 변수.
	bool CanAttack = true;
	
	// 대시 쿨타임 타이머 변수.
	FTimerHandle DashCoolDownTimerHandle;
	
	// 대시 가능 여부.
	bool bCanDash = true;
	
	// 대시 쿨타임.
	float DashCooldown = 2.0f;

	bool bShouldRotateToTarget = false;
	FVector RotateTargetLocation;
};
