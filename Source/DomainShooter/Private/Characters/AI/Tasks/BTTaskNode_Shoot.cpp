// Domain Shooter By Brian


#include "Characters/AI/Tasks/BTTaskNode_Shoot.h"
#include "AIController.h"
#include "Characters/AICharacter.h"



UBTTaskNode_Shoot::UBTTaskNode_Shoot()
{
    NodeName = "Shoot";
}

EBTNodeResult::Type UBTTaskNode_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* DomainAIController = OwnerComp.GetAIOwner();

    if (!DomainAIController)
    {
        return EBTNodeResult::Failed;
    }

    AAICharacter* DomainAICharacter = Cast<AAICharacter>(DomainAIController->GetPawn());
    if (!DomainAICharacter)
    {
        return EBTNodeResult::Failed;
    }

    // call shoot for weapon
    DomainAICharacter->ShootTask();

    return EBTNodeResult::Succeeded;
}
