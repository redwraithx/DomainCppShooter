// Domain Shooter By Brian


#include "Characters/AI/DomainAiBaseController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Characters/PlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"


ADomainAiBaseController::ADomainAiBaseController()
{
	PrimaryActorTick.bCanEverTick = true;

	AI_PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI_PerceptionComponent"));

	AI_SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI_SightConfig"));
	AI_SightConfig->SightRadius = 1500.f;
	AI_SightConfig->LoseSightRadius = 2000.f;
	AI_SightConfig->PeripheralVisionAngleDegrees = 90.f;
	
	AI_SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AI_SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AI_SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AI_HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AI_HearingConfig"));
	AI_HearingConfig->HearingRange = 3000.f;
	AI_HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	AI_HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AI_HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	

	AI_PerceptionComponent->ConfigureSense(*AI_SightConfig);
	AI_PerceptionComponent->ConfigureSense(*AI_HearingConfig);
	AI_PerceptionComponent->SetDominantSense(AI_SightConfig->GetSenseImplementation()); // only need 1 dominate sense

	//AI_PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ADomainAiBaseController::OnPerceptionUpdated);
	AI_PerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ADomainAiBaseController::OnTargetPerceptionInfoUpdated);
}

void ADomainAiBaseController::BeginPlay()
{
	Super::BeginPlay();

	if (BT_BaseAiBehaviorTree)
	{
		RunBehaviorTree(BT_BaseAiBehaviorTree);
		BB_BaseAiBlackboard = GetBlackboardComponent();
		
		if (BB_BaseAiBlackboard)
		{
			bool bBlackboard = UseBlackboard(BB_BaseAiBlackboard->GetBlackboardAsset(), BB_BaseAiBlackboard);

			
			if (bBlackboard)
			{
				FVector TargetLocation = GetPawn()->GetActorLocation();
		
				BB_BaseAiBlackboard->SetValueAsVector(TEXT("StartLocation"), TargetLocation);
				
			}
		}
	}
}

void ADomainAiBaseController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ADomainAiBaseController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (Actor->IsA(APlayerCharacter::StaticClass()))
		{
			FActorPerceptionBlueprintInfo PerceptionBlueprintInfo;
			AI_PerceptionComponent->GetActorsPerception(Actor, PerceptionBlueprintInfo);
			
			for (const FAIStimulus& Stimulus : PerceptionBlueprintInfo.LastSensedStimuli)
			{
				if (Stimulus.WasSuccessfullySensed())
				{
					BB_BaseAiBlackboard->SetValueAsVector(TEXT("TargetLocation"), Actor->GetActorLocation());

					return;
				}
				
			}
		}
	}
	BB_BaseAiBlackboard->ClearValue(TEXT("TargetLocation"));
}

void ADomainAiBaseController::OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	AActor* Actor = UpdateInfo.Target.Get();
	
	if (Actor && Actor->IsA(APlayerCharacter::StaticClass()))
	{
		if (UpdateInfo.Stimulus.WasSuccessfullySensed())
		{
			if (UpdateInfo.Stimulus.Type.Name == "Hearing")
			{
				BB_BaseAiBlackboard->SetValueAsVector(TEXT("HearingLocation"), UpdateInfo.Stimulus.StimulusLocation);
				
			}
			/*else
			{
				BB_BaseAiBlackboard->ClearValue(TEXT("HearingLocation"));
			}*/

			// sight
			BB_BaseAiBlackboard->SetValueAsVector(TEXT("TargetLocation"), Actor->GetActorLocation());
			BB_BaseAiBlackboard->SetValueAsObject(TEXT("TargetActor"), Actor);
			SetFocus(Actor);
		}
		else
		{
			BB_BaseAiBlackboard->ClearValue(TEXT("TargetLocation"));
			BB_BaseAiBlackboard->ClearValue(TEXT("TargetActor"));
			BB_BaseAiBlackboard->ClearValue(TEXT("HearingLocation"));
			ClearFocus(EAIFocusPriority::Gameplay);
		}
	}
}
