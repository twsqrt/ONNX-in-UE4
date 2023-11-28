#include "TempActor.h"
#include "Modules/ModuleManager.h"
#include "ONNX.h"

ATempActor::ATempActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATempActor::BeginPlay()
{
	Super::BeginPlay();

	FString ContentDir = FPaths::ProjectContentDir();
	FString ModelPath = FPaths::Combine(*ContentDir, TEXT("Models/linear.onnx"));

	FONNXModule* ONNX = FModuleManager::GetModulePtr<FONNXModule>("ONNX");
	ONNX->RunLinearRegression(ModelPath);
}
