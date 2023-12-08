// Copyright Epic Games, Inc. All Rights Reserved.

#include "ONNX.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/UnrealMemory.h"
#include <vector>

THIRD_PARTY_INCLUDES_START
#include "onnxruntime_cxx_api.h"
THIRD_PARTY_INCLUDES_END

#define LOCTEXT_NAMESPACE "FONNXModule"

void FONNXModule::StartupModule()
{

#ifndef PLATFORM_WINDOWS
#error "Plugin is designed for windows platform only!"
#endif

	FString BaseDir = IPluginManager::Get().FindPlugin("ONNX")->GetBaseDir();
	FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/onnxruntime.dll"));

	DLLHandler = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (!DLLHandler)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ONNX library error!", "Failed to load ONNX third party library"));
	}

	SessionIsOpen = false;
}

void FONNXModule::ShutdownModule()
{
	CloseModel();

	FPlatformProcess::FreeDllHandle(DLLHandler);
	DLLHandler = nullptr;
}

void* UEAlloc(struct OrtAllocator* this_, size_t size)
{
	return FMemory::Malloc(size);
}

void UEFree(struct OrtAllocator* this_, void* p)
{
	FMemory::Free(p);
}

void FONNXModule::LoadModel(FString ModelPath)
{
	CurrentEnv = static_cast<Ort::Env*>(FMemory::Malloc(sizeof(Ort::Env)));
	CurrentSession = static_cast<Ort::Session*>(FMemory::Malloc(sizeof(Ort::Session)));

	new (CurrentEnv) Ort::Env(ORT_LOGGING_LEVEL_FATAL, "Default"); 
	new (CurrentSession) Ort::Session{ *CurrentEnv, TCHAR_TO_WCHAR(*ModelPath), Ort::SessionOptions() };

	SessionIsOpen = true;
}

int32 FONNXModule::Predict(float SepalLength, float SepalWidth, float PetalLength, float PetalWidth)
{
	OrtAllocator UEAllocator{ ORT_API_VERSION, UEAlloc, UEFree };

	char* InputName = CurrentSession->GetInputName(0, &UEAllocator);
	char* OutputName = CurrentSession->GetOutputName(0, &UEAllocator);
	std::vector<int64> InputShape = CurrentSession->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
	std::vector<int64> OutputShape = CurrentSession->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();

	float InputValues[] = { SepalLength, SepalWidth, PetalLength, PetalWidth };

	Ort::MemoryInfo MemoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	Ort::Value InputTensor = Ort::Value::CreateTensor(MemoryInfo, InputValues, 4, InputShape.data(), InputShape.size());
	const char* InputNamePtr[] = { InputName };
	const char* OutputNamePtr[] = { OutputName };

	std::vector<Ort::Value> Outputs = CurrentSession->Run(Ort::RunOptions{ nullptr }, InputNamePtr, &InputTensor, 1, OutputNamePtr, 1);

	Ort::Value& Output = Outputs[0];
	int32 Result = Output.GetTensorMutableData<int32>()[0];

	FMemory::Free(InputName);
	FMemory::Free(OutputName);

	return Result;
}

FString FONNXModule::GetName(int32 IrisId)
{
	switch (IrisId)
	{
	case 0:
		return FString("Setosa");
	case 1:
		return FString("Versicolour");
	case 2:
		return FString("Virginica");
	default:
		return FString("None");
	}
}

void FONNXModule::CloseModel()
{
	if (SessionIsOpen)
	{
		FMemory::Free(CurrentSession);
		FMemory::Free(CurrentEnv);

		SessionIsOpen = false;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FONNXModule, ONNX)