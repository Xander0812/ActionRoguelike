// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include <Kismet/KismetMathLibrary.h>



// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASCharacter::Look);

		// Primary attack
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryAttack);

		//Blackole ability
		EnhancedInputComponent->BindAction(BlackHoleAbilityAction, ETriggerEvent::Triggered, this, &ASCharacter::BlackHoleAbility);

		//Teleport ability
		EnhancedInputComponent->BindAction(TeleportAbilityAction, ETriggerEvent::Triggered, this, &ASCharacter::TeleportAbility);

		//Interaction with the world
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryInteract);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	//UE_LOG(LogTemp, Display, TEXT("OrientRotationToMovement:"), GetCharacterMovement()->bOrientRotationToMovement);

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);
	
	//we wait for attack animation to finish before spawning the projectile
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);
}

void ASCharacter::BlackHoleAbility()
{
	PlayAnimMontage(AttackAnim);

	//we wait for attack animation to finish before spawning the projectile
	GetWorldTimerManager().SetTimer(TimerHandle_BlackHole, this, &ASCharacter::BlackHoleAbility_TimeElapsed, 0.2f);
}

void ASCharacter::TeleportAbility()
{
	PlayAnimMontage(AttackAnim);

	//we wait for attack animation to finish before spawning the projectile
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, this, &ASCharacter::TeleportAbility_TimeElapsed, 0.2f);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	ASCharacter::SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAbility_TimeElapsed()
{
	ASCharacter::SpawnProjectile(BlackHoleClass);
}

void ASCharacter::TeleportAbility_TimeElapsed()
{
	ASCharacter::SpawnProjectile(TeleportClass);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if (ensure(ClassToSpawn))
	{
		//Set the point in the model from whitch shoud projectile fire from
		FVector _handLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		FCollisionShape _colliderShape;
		_colliderShape.SetSphere(20.f);

		FCollisionQueryParams _querryParams;
		_querryParams.AddIgnoredActor(this);

		//Set query parameters
		FCollisionObjectQueryParams _objectQueryParams;
		_objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		_objectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		_objectQueryParams.AddObjectTypesToQuery(ECC_Pawn);


		//setup camera rotation and position for ray to fire from
		FVector _eyeLocation = CameraComp->GetComponentLocation();
		FRotator _eyeRotation = CameraComp->GetComponentRotation();

		//spawn param defaults
		FActorSpawnParameters _spawnParams;
		_spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		_spawnParams.Instigator = this;

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
		FRotator _projectileRotation = (_traceEnd - _handLocation).Rotation();
		FTransform _spawnTM = FTransform(_projectileRotation, _handLocation);
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, _spawnTM, _spawnParams);
	}
}


void ASCharacter::PrimaryInteract()
{
	if (InteractionComp) 
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstagatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}

