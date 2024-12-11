// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams _objectQueryParams;
	_objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* _myOwner = GetOwner();

	FVector _eyeLocation;
	FRotator _eyeRotation;
	_myOwner->GetActorEyesViewPoint(_eyeLocation, _eyeRotation);

	FVector _end = _eyeLocation + (_eyeRotation.Vector() * 1000);

	//FHitResult _hit;
	//bool _blockingHit = GetWorld()->LineTraceSingleByObjectType(_hit, _eyeLocation, _end, _objectQueryParams);

	TArray<FHitResult> _hits;

	float _radius = 30.f;

	FCollisionShape _shape;
	_shape.SetSphere(_radius);

	bool _blockingHit = GetWorld()->SweepMultiByObjectType(_hits, _eyeLocation, _end, FQuat::Identity, _objectQueryParams, _shape);

	FColor _lineColor = _blockingHit ? FColor::Green : FColor::Red;

	for(FHitResult _hit : _hits)
	{
		AActor* _hitActor = _hit.GetActor();
		if (_hitActor)
		{
			if (_hitActor->Implements<USGameplayInterface>())
			{
				APawn* _mypawn = Cast<APawn>(_myOwner);

				ISGameplayInterface::Execute_Interact(_hitActor, _mypawn);

				break;
			}
		}

		DrawDebugSphere(GetWorld(), _hit.ImpactPoint, _radius, 32, _lineColor, false, 2.0f);
	}

	DrawDebugLine(GetWorld(), _eyeLocation, _end, _lineColor, false, 2.0f, 0, 2.0f);

}

