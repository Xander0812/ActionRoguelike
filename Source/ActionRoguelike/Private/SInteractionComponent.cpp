// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteracion(TEXT("ar.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component."), ECVF_Cheat);


// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.f;
	TraceDistance = 500.f;
	CollisionChannel = ECC_WorldDynamic;
	// ...
}

void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* MyPawn = Cast<APawn>(GetOwner());
	if(MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

void USInteractionComponent::FindBestInteractable()
{
	bool _bDebugDraW = CVarDebugDrawInteracion.GetValueOnGameThread();

	FCollisionObjectQueryParams _objectQueryParams;
	_objectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* _myOwner = GetOwner();

	//Set ray start position and rotation
	FVector _eyeLocation;
	FRotator _eyeRotation;
	_myOwner->GetActorEyesViewPoint(_eyeLocation, _eyeRotation);

	//Set ray end point
	FVector _end = _eyeLocation + (_eyeRotation.Vector() * TraceDistance);

	//We need an array of hits because we are working with a shape instead of ray
	TArray<FHitResult> _hits;


	//we need to setup collision shape instead of ray so our interaction would be less precise. We wouldn't need to look directly at the object to interact
	FCollisionShape _shape;
	_shape.SetSphere(TraceRadius);

	bool _blockingHit = GetWorld()->SweepMultiByObjectType(_hits, _eyeLocation, _end, FQuat::Identity, _objectQueryParams, _shape);

	//change color of the debugging colore depending on the result
	FColor _lineColor = _blockingHit ? FColor::Green : FColor::Red;

	//clear focused actor before trying to fill
	FocusedActor = nullptr;

	for (FHitResult _hit : _hits)
	{
		if (_bDebugDraW)
			DrawDebugSphere(GetWorld(), _hit.ImpactPoint, TraceRadius, 32, _lineColor, false, 2.0f);


		AActor* _hitActor = _hit.GetActor();
		if (_hitActor)
		{
			//Check if the actor we hit has Gamplay interface implemented
			//(if the object has interact function)
			if (_hitActor->Implements<USGameplayInterface>())
			{
				FocusedActor = _hitActor;
				break;
			}
		}
	}

	if(FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else 
	{
		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (_bDebugDraW)
		DrawDebugLine(GetWorld(), _eyeLocation, _end, _lineColor, false, 2.0f, 0, 2.0f);
}

void USInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void USInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "NoFocusActorToInteract");
		return;
	}

	APawn* _mypawn = Cast<APawn>(GetOwner());

	ISGameplayInterface::Execute_Interact(InFocus, _mypawn);
}
