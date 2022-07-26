// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/BoxComponent.h"
#include "PlayerCharacter.h" 
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Engine/Engine.h"
 

// Sets default values
AEnemy::AEnemy() 
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	DamageCollision->SetupAttachment(RootComponent);

	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	// Enemy Field of View
	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 120.0f;

	// Can Detect All
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;  
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// forget Enemy after 0,1sek
	SightConfig->SetMaxAge(0.1f);

	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &AEnemy::OnSensed);

	CurrentVelocity = FVector::ZeroVector;
	MovementSpeed = 200.0f;

	DistanceSquared = BIG_NUMBER;

	

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{

	Super::BeginPlay();

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHit);
	 
	BaseLocation = this->GetActorLocation();

	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentVelocity.IsZero() && followSensed && isAlive)
	{
		NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;

		if (BackToBaseLocation) 
		{
			if ((NewLocation - BaseLocation).SizeSquared2D() < DistanceSquared)
			{
				DistanceSquared = (NewLocation - BaseLocation).SizeSquared2D();
			}
			else
			{
				CurrentVelocity = FVector::ZeroVector;
				DistanceSquared = BIG_NUMBER;
				BackToBaseLocation = false;

				SetNewRotation(GetActorForwardVector(), GetActorLocation());
			}

		}

		SetActorLocation(NewLocation);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{

	APlayerCharacter* Char = Cast<APlayerCharacter>(OtherActor);
	

	if (Char && isAlive)
	{
		//if (GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Damage to Player!"));

		Char->DealDamage(DamageValue, EnemyRotation, 100);
	}

}

void AEnemy::OnSensed(const TArray<AActor*>& UpdatedActors)
{

	for (int i = 0; i < UpdatedActors.Num(); i++) 
	{

		FActorPerceptionBlueprintInfo Info;
		AIPerComp->GetActorsPerception(UpdatedActors[i], Info);


		// Go to Sensed Actor
		if (Info.LastSensedStimuli[0].WasSuccessfullySensed())
		{
			// DirectionVector to Player = PlayerLocation - EnemyLocation;
			FVector dir = UpdatedActors[i]->GetActorLocation() - GetActorLocation();
			dir.Z = 0.0f;

			// Velocity = DirectionVectorLength * Speed
			CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;

			// Update Rotation towards Player
			SetNewRotation(UpdatedActors[i]->GetActorLocation(), GetActorLocation());

		}
		// Go back to base
		else
		{
			FVector dir = BaseLocation - GetActorLocation();
			dir.Z = 0.0f;

			if (dir.SizeSquared2D() > 1.0f) 
			{
				CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;
				BackToBaseLocation = true;

				SetNewRotation(BaseLocation, GetActorLocation());
			}

		}
	}


}

void AEnemy::SetNewRotation(FVector TargetPosition, FVector CurrentPosition)
{
	// Operator Overload
	FVector NewDirection = TargetPosition - CurrentPosition;
	NewDirection.Z = 0.0f;

	EnemyRotation = NewDirection.Rotation();

	SetActorRotation(EnemyRotation);

}

void AEnemy::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		// TODO: Death Animation
		//Destroy();
		isAlive = false; 
		DisableInput(GetWorld()->GetFirstPlayerController());
		
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetPhysicsBlendWeight(1.0);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
		
		
	}  
}

