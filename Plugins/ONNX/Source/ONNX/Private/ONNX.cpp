// Copyright Epic Games, Inc. All Rights Reserved.

#include "ONNX.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

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

int32 FONNXModule::GetModelInputCount()
{
	const wchar_t* ModelPath = L"C:\\Users\\twsqrt\\source\\repos\\ONNX-in-UE4\\Plugins\\ONNX\\Resources\\bidaf-9.onnx";

	Ort::Env Env(ORT_LOGGING_LEVEL_FATAL, "Default");
	Ort::SessionOptions Options;
	Ort::Session Session(Env, ModelPath, Options);

	return Session.GetInputCount();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FONNXModule, ONNX)