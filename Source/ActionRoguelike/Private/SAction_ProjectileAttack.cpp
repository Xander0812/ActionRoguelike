// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"
#include "GameFramework/Character.h"
#include "SActionComponent.h"


USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::Initialize(USActionComponent* NewActionComp)
{
	Super::Initialize(NewActionComp);

	FActorSpawnParameters _spawnParams;
	_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_spawnParams.Instigator = Cast<ACharacter>(NewActionComp->GetOwner());

	//FTransform _spawnTM = FTransform(FVector::ZeroVector);


	for(int i = 0; i < ProjectileArraySize; i++)
	{
		ASProjectileBaseClass* _spawnedProjectile = Cast<ASProjectileBaseClass>(GetWorld()->SpawnActor<AActor>(ProjectileClass, FVector::ZeroVector, FRotator::ZeroRotator, _spawnParams));

		if(_spawnedProjectile)
		{
			ProjectileArray.Add(_spawnedProjectile);
		}
	}
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


void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{

	if (ensure(ProjectileClass))
	{
		ASProjectileBaseClass* _pickedProjectile = GetFreeProjectile();
		_pickedProjectile->SetActorTransform(GetProjectileDirection(InstigatorCharacter));
		_pickedProjectile->ActivateProjectile();
	}

	StopAction(InstigatorCharacter);
}

FTransform USAction_ProjectileAttack::GetProjectileDirection(ACharacter* InstigatorCharacter)
{
	//Set the point in the model from whitch shoud projectile fire from
	FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

	FCollisionShape _colliderShape;
	_colliderShape.SetSphere(20.f);

	FCollisionQueryParams _querryParams;
	_querryParams.AddIgnoredActor(InstigatorCharacter);

	//Set query parameters
	FCollisionObjectQueryParams _objectQueryParams;
	_objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	_objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	_objectQueryParams.AddObjectTypesToQuery(ECC_Pawn);



	//setup camera rotation and position for ray to fire from
	FVector _eyeLocation; // = FVector(InstigatorCharacter->GetPawnViewLocation().X + 150, InstigatorCharacter->GetPawnViewLocation().Y, InstigatorCharacter->GetPawnViewLocation().Z);
	FRotator _eyeRotation; // = InstigatorCharacter->GetControlRotation();

	InstigatorCharacter->GetActorEyesViewPoint(_eyeLocation, _eyeRotation);
	//_eyeLocation = FVector(_eyeLocation.X, _eyeLocation.Y, _eyeLocation.Z);

	//spawn param defaults
	FActorSpawnParameters _spawnParams;
	_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	_spawnParams.Instigator = InstigatorCharacter;

	//end  and start of a ray
	FVector _traceEnd = _eyeLocation + (_eyeRotation.Vector() * 5000);
	FVector _traceStart = _eyeLocation + (_eyeRotation.Vector() * 150);

	//raycast
	FHitResult _hit;
	bool _blockingHit = GetWorld()->SweepSingleByObjectType(_hit, _traceStart, _traceEnd, FQuat::Identity, _objectQueryParams, _colliderShape, _querryParams);


	if (_blockingHit)
	{
		_traceEnd = _hit.Location;
	}

	//adjust projectile rotation so it would fly right in the center of the screen where player aims and spawn projectile
	FRotator _projectileRotation = (_traceEnd - HandLocation).Rotation();
	FTransform _spawnTM = FTransform(_projectileRotation, HandLocation);

	return _spawnTM;
}

ASProjectileBaseClass* USAction_ProjectileAttack::GetFreeProjectile()
{
	for(ASProjectileBaseClass* _projectile : ProjectileArray)
	{
		if(!_projectile->IsProjectileActive())
		{
			return _projectile;
		}
	}
	return ProjectileArray[0];
}
