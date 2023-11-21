#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TempActor.generated.h"

UCLASS()
class ONNXTEST_API ATempActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATempActor();
protected:
	virtual void BeginPlay() override;
};
