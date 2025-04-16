// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include <SProjectileBaseClass.h>


USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpreadPitch = 3;
	MaxBulletSpreadYaw = 3;
	ShootSocketName = "Muzzle_01";
}

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* _controller = OwnerComp.GetAIOwner();

	if(ensure(_controller))
	{
		ACharacter* _pawn = Cast<ACharacter>(_controller->GetPawn());

		if(_pawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		FVector _muzzleLocation = _pawn->GetMesh()->GetSocketLocation(ShootSocketName);

		AActor* _targetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));

		if(_targetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if(!USAttributeComponent::IsActorAlive(_targetActor))
		{
			return EBTNodeResult::Failed;
		}

		//VFX

		UGameplayStatics::SpawnEmitterAttached(ParticleEffect, _pawn->GetMesh(), ShootSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		_pawn->PlayAnimMontage(AttackAnim);


		//Cast ranged attack with accuracy setting when we see player 

		FVector _targetDirection = _targetActor->GetActorLocation() - _muzzleLocation;
		FRotator _muzzleRotation = _targetDirection.Rotation();

		_muzzleRotation.Pitch += FMath::RandRange(0.f, MaxBulletSpreadPitch);
		_muzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpreadYaw, MaxBulletSpreadYaw);

		FActorSpawnParameters  _spawnParams;
		_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_spawnParams.Instigator = _pawn;

		//AActor* _newProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, _muzzleLocation, _muzzleRotation, _spawnParams);
		ASProjectileBaseClass* _spawnedProjectile = Cast<ASProjectileBaseClass>(GetWorld()->SpawnActor<AActor>(ProjectileClass, _muzzleLocation, _muzzleRotation, _spawnParams));
		_spawnedProjectile->ActivateProjectile();


		return _spawnedProjectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}


