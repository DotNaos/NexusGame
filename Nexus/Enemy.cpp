// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/BoxComponent.h"
#include "PlayerCharacter.h" 
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// Sets default values
AEnemy::AEnemy() 
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(RootComponent);


	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// forget Enemy after 0,1sek
	SightConfig->SetMaxAge(0.1f);

	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &AEnemy::OnSensed);

	CurrentVelocity = FVector::ZeroVector;
	MovementSpeed = 375.0f;

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

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{

}

void AEnemy::OnSensed(const TArray<AActor*>& UpdatedActors)
{
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
}

