// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileBaseClass.h"
	
// Sets default values
ASProjectileBaseClass::ASProjectileBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("NoCollision");
	RootComponent = SphereComp;

	BaseParticleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	BaseParticleEffectComp->SetupAttachment(SphereComp);
	BaseParticleEffectComp->SetVisibility(false);

	ImpactParticleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("ImpactEffectComp");
	ImpactParticleEffectComp->SetupAttachment(SphereComp);
	ImpactParticleEffectComp->bAutoActivate = false;
	ImpactParticleEffectComp->bAutoDestroy = false;

	ExtraParticleEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("ExtraParticleEffectComp");
	ExtraParticleEffectComp->SetupAttachment(SphereComp);
	ExtraParticleEffectComp->bAutoActivate = false;
	ExtraParticleEffectComp->bAutoDestroy = false;

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(SphereComp);


	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0;

	bReplicates = true;
	MovementComp->InitialSpeed = 0;
}

// Called when the game starts or when spawned
void ASProjectileBaseClass::BeginPlay()
{
	Super::BeginPlay();

	//UGameplayStatics::SpawnSoundAttached(LoopSoundCue, this->GetRootComponent(), TEXT("SphereComp"), GetActorLocation(), EAttachLocation::KeepRelativeOffset, true);
	
	//We need to ignore instagator (who spawned the projectile) to not generate hit and overlap events
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ASProjectileBaseClass::DeactivateProjectile()
{
	BaseParticleEffectComp->SetVisibility(false);
	BaseParticleEffectComp->Deactivate();

	SphereComp->SetCollisionProfileName("NoCollision");

	MovementComp->StopMovementImmediately();
	MovementComp->SetVelocityInLocalSpace(FVector(0, 0, 0));

	if(GetWorldTimerManager().IsTimerActive(TimerHandle_ProjectileLifetime))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_ProjectileLifetime);
	}
}

void ASProjectileBaseClass::ActivateProjectile()
{
	SphereComp->SetCollisionProfileName("Projectile");

	MovementComp->SetVelocityInLocalSpace(FVector(3000, 0, 0));

	AudioComp->SetSound(LoopSoundCue);
	AudioComp->Activate();

	if(ProjectileLifetime > 0)
		GetWorldTimerManager().SetTimer(TimerHandle_ProjectileLifetime, this, &ASProjectileBaseClass::DeactivateProjectile, ProjectileLifetime);

	BaseParticleEffectComp->Activate();
	BaseParticleEffectComp->SetVisibility(true);
}

bool ASProjectileBaseClass::IsProjectileActive() const
{
	return BaseParticleEffectComp->GetVisibleFlag();
}

void ASProjectileBaseClass::OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundCue, GetActorLocation());
		//We spawn Emmiter to play animation of teleportation

		FVector _hitLocation = SweepResult.ImpactPoint;
		FRotator _impactRotation = (SphereComp->GetComponentTransform().GetLocation() - SweepResult.ImpactPoint).Rotation();
		FTransform _newImpactTransform = FTransform(_impactRotation, _hitLocation);

		ImpactParticleEffectComp->SetWorldTransform(_newImpactTransform, false, nullptr, ETeleportType::ResetPhysics);
		ImpactParticleEffectComp->Deactivate();
		ImpactParticleEffectComp->Activate();
		AudioComp->SetSound(ImpactSoundCue);

		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExtraParticleEffect, SweepResult.ImpactPoint, GetActorRotation(), true);
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeEffect, GetActorLocation(), /*CameraShakeInnerRadius*/ 100, /*CameraShakeOuterRadius*/ 500);
}

// Called every frame
void ASProjectileBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

