// Fill out your copyright notice in the Description page of Project Settings.


#include "SApplyEffectZone.h"
#include "SCharacter.h"
// Sets default values
ASApplyEffectZone::ASApplyEffectZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>("Box");
	RootComponent = BoxComp;
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASApplyEffectZone::OnBoxBeginOverlap);

	DecalComp = CreateDefaultSubobject<UDecalComponent>("Decal");
	DecalComp->SetupAttachment(BoxComp);
}

void ASApplyEffectZone::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ASCharacter* _character = CastChecked<ASCharacter>(OtherActor);
	//USActionComponent* _a


	//if(!_character || !_character->IsValidLowLevel() || !_character->HasAuthority())
	//{
	//	return;
	//}

	USActionComponent* _actionComp = USActionComponent::GetActionComponent(OtherActor);

	if(_actionComp)
	{
		_actionComp->AddAction(this, EffectClassToApply);
	}
}

// Called when the game starts or when spawned
void ASApplyEffectZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASApplyEffectZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

