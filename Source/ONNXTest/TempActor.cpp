#include "TempActor.h"
#include "Modules/ModuleManager.h"
#include "Containers/Array.h"
#include "ONNX.h"

ATempActor::ATempActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATempActor::BeginPlay()
{
	Super::BeginPlay();

	FONNXModule* ONNX = FModuleManager::GetModulePtr<FONNXModule>("ONNX");

	FString ContentDir = FPaths::ProjectContentDir();
	FString ModelPath = FPaths::Combine(*ContentDir, TEXT("Models/iris.onnx"));
	ONNX->LoadModel(ModelPath);

	float SepalLength = 5.8;
	float SepalWidth = 2.7;
	float PetalLength = 5.1;
	float PetalWidth = 1.9;

	FString InputValuesMessage = FString::Printf(
		TEXT("Sepal L: %f, Sepal W: %f, Petal L: %f, Petal W: %f"), 
		SepalLength,
		SepalWidth,
		PetalLength,
		PetalWidth
	);
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::White, InputValuesMessage);

	int32 IrisId = ONNX->Predict(SepalLength, SepalWidth, PetalLength, PetalWidth);
	FString IrisName = ONNX->GetName(IrisId);
	FString ResultMessage = FString::Printf(TEXT("Model Result: %s"), *IrisName);
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::White, ResultMessage);

	ONNX->CloseModel();
}
