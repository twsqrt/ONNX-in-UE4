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

	FONNXModule* Plugin = FModuleManager::GetModulePtr<FONNXModule>("ONNX");
	UE_LOG(LogTemp, Display, TEXT("I'm trying to load a model."));
	int32 ModelInputCount = Plugin->GetModelInputCount();
	UE_LOG(LogTemp, Display, TEXT("Model loaded. Model input count = %d"), ModelInputCount);
}
