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

void USInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams _objectQueryParams;
	_objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* _myOwner = GetOwner();

	//Set ray start position and rotation
	FVector _eyeLocation;
	FRotator _eyeRotation;
	_myOwner->GetActorEyesViewPoint(_eyeLocation, _eyeRotation);

	//Set ray end point
	FVector _end = _eyeLocation + (_eyeRotation.Vector() * 1000);

	//We need an array of hits because we are working with a shape instead of ray
	TArray<FHitResult> _hits;

	float _radius = 30.f;

	//we need to setup collision shape instead of ray so our interaction would be less precise. We wouldn't need to look directly at the object to interact
	FCollisionShape _shape;
	_shape.SetSphere(_radius);

	bool _blockingHit = GetWorld()->SweepMultiByObjectType(_hits, _eyeLocation, _end, FQuat::Identity, _objectQueryParams, _shape);

	//change color of the debugging colore depending on the result
	FColor _lineColor = _blockingHit ? FColor::Green : FColor::Red;

	for(FHitResult _hit : _hits)
	{
		AActor* _hitActor = _hit.GetActor();
		if (_hitActor)
		{
			//Check if the actor we hit has Gamplay interface implemented
			//(if the object has interact function)
			if (_hitActor->Implements<USGameplayInterface>())
			{
				APawn* _mypawn = Cast<APawn>(_myOwner);

				ISGameplayInterface::Execute_Interact(_hitActor, _mypawn);

				DrawDebugSphere(GetWorld(), _hit.ImpactPoint, _radius, 32, _lineColor, false, 2.0f);
				break;
			}
		}

	}

	DrawDebugLine(GetWorld(), _eyeLocation, _end, _lineColor, false, 2.0f, 0, 2.0f);

}

