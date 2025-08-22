// Domain Shooter By Brian

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DomainAiBaseController.generated.h"


// forward declarations
class UBehaviorTree;
class UBehaviorTreeComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
struct FActorPerceptionUpdateInfo;



/**
 * 
 */
UCLASS()
class DOMAINSHOOTER_API ADomainAiBaseController : public AAIController
{
	GENERATED_BODY()

public:
	ADomainAiBaseController();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


private:
	// AI | Perception
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BT_BaseAiBehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UBlackboardComponent* BB_BaseAiBlackboard;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AI_PerceptionComponent;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> AI_SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
	TObjectPtr<UAISenseConfig_Hearing> AI_HearingConfig;

	// Callback function for perception updates
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
};
