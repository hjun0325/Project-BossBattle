// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_UltimateShotArrow.h"

void UAnimNotify_UltimateShotArrow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
}
