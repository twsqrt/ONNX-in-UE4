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
}

void FONNXModule::ShutdownModule()
{
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

void FONNXModule::RunLinearRegression(FString ModelPath)
{
	Ort::Env Env(ORT_LOGGING_LEVEL_FATAL, "Default");
	Ort::Session Session{ Env, TCHAR_TO_WCHAR(*ModelPath), Ort::SessionOptions()};
	OrtAllocator UEAllocator{ ORT_API_VERSION, UEAlloc, UEFree };

	const char* InputName = Session.GetInputName(0, &UEAllocator);
	const char* OutputName = Session.GetOutputName(0, &UEAllocator);
	std::vector<int64> InputShape = Session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
	std::vector<int64> OutputShape = Session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();

	std::vector<float> InputValues = { 1.0f, 2.0f, 3.0f };

	Ort::MemoryInfo MemoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	Ort::Value InputTensor = Ort::Value::CreateTensor(MemoryInfo, InputValues.data(), InputValues.size(), InputShape.data(), InputShape.size());
	const char* const* InputNamePtr = &InputName;
	const char* const* OutputNamePtr = &OutputName;
	std::vector<Ort::Value> Outputs = Session.Run(Ort::RunOptions{ nullptr }, InputNamePtr, &InputTensor, 1, OutputNamePtr, 1);

	Ort::Value& Output = Outputs[0];
	const float* Results = Output.GetTensorMutableData<float>();
	int32 ResultsCount = Output.GetTensorTypeAndShapeInfo().GetElementCount();

	for (int32 i = 0; i < ResultsCount; i++)
	{
		FString Message = FString::Printf(TEXT("Input: %f, Result: %f"), InputValues[i], Results[i]);
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::White, Message);
	}

	FMemory::Free((void*)InputName);
	FMemory::Free((void*)OutputName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FONNXModule, ONNX)