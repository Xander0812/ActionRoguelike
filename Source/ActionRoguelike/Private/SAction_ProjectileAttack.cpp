// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"
#include "GameFramework/Character.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* _character = Cast<ACharacter>(Instigator);
	if(_character)
	{
		_character -> PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(ProjectileClass.GetDefaultObject()->CastParticleEffect, _character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		if (_character->HasAuthority())
		{
			FTimerHandle _timerHandle_AttackDelay;
			FTimerDelegate _delegate;
			_delegate.BindUFunction(this, "AttackDelay_Elapsed", _character);

			GetWorld()->GetTimerManager().SetTimer(_timerHandle_AttackDelay, _delegate, AttackAnimDelay, false);
		}
	}
	//spawn cast effect depending on projectile class
}


void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstagatorCharacter)
{

	if (ensure(ProjectileClass))
	{
		//Set the point in the model from whitch shoud projectile fire from

		FVector HandLocation = InstagatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		FCollisionShape _colliderShape;
		_colliderShape.SetSphere(20.f);

		FCollisionQueryParams _querryParams;
		_querryParams.AddIgnoredActor(InstagatorCharacter);

		//Set query parameters
		FCollisionObjectQueryParams _objectQueryParams;
		_objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		_objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		_objectQueryParams.AddObjectTypesToQuery(ECC_Pawn);


		//setup camera rotation and position for ray to fire from
		FVector _eyeLocation = InstagatorCharacter->GetPawnViewLocation();
		FRotator _eyeRotation = InstagatorCharacter->GetControlRotation();

		//spawn param defaults
		FActorSpawnParameters _spawnParams;
		_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_spawnParams.Instigator = InstagatorCharacter;

		//end of a ray
		FVector _traceEnd = _eyeLocation + (_eyeRotation.Vector() * 5000);

		//raycast
		FHitResult _hit;
		bool _blockingHit = GetWorld()->SweepSingleByObjectType(_hit, _eyeLocation, _traceEnd, FQuat::Identity, _objectQueryParams, _colliderShape, _querryParams);

		if (_blockingHit)
		{
			_traceEnd = _hit.Location;
		}

		//adjust projectile rotation so it would fly right in the center of the screen where player aims and spawn projectile
		FRotator _projectileRotation = (_traceEnd - HandLocation).Rotation();
		FTransform _spawnTM = FTransform(_projectileRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, _spawnTM, _spawnParams);
	}

	StopAction(InstagatorCharacter);
}
