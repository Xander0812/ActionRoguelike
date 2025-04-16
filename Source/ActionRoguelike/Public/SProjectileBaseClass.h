// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SAttributeComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include <Kismet/GameplayStatics.h>
#include "SProjectileBaseClass.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASProjectileBaseClass : public AActor
{
	GENERATED_BODY()
	
public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//We need sphere component to track overlap and hit events for projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	//built in UE projectile movement component to adjust projectaile like behaviour
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MovementComp;

	//particle effect based visualisation of projectiles from downloaded assets
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* BaseParticleEffectComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ImpactParticleEffectComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ExtraParticleEffectComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* ImpactAudioComponent;

	/* VFX of projectile casting. Initiliser gets it as reference */
	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* CastParticleEffect;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TSubclassOf<UCameraShakeBase> CameraShakeEffect;

	UPROPERTY(EditAnywhere, Category = "SFX")
	USoundCue* LoopSoundCue;

	UPROPERTY(EditAnywhere, Category = "SFX")
	USoundCue* ImpactSoundCue;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ProjectileLifetime;

	FTimerHandle TimerHandle_ProjectileLifetime;

	UFUNCTION()
	virtual void DeactivateProjectile();

	UFUNCTION()
	virtual void ActivateProjectile();

	UFUNCTION()
	bool IsProjectileActive() const;

	UFUNCTION()
	virtual void OnComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Sets default values for this actor's properties
	ASProjectileBaseClass();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
