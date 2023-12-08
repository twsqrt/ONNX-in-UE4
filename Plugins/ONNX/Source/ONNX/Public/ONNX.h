// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace Ort
{
	struct Session;
	struct Env;
}

class ONNX_API FONNXModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void LoadModel(FString ModelPath);
	int32 Predict(float SepalLength, float SepalWidth, float PetalLength, float PetalWidth);
	FString GetName(int32 IrisId);
	void CloseModel();
private:
	void* DLLHandler;
	Ort::Session* CurrentSession;
	Ort::Env* CurrentEnv;
	bool SessionIsOpen;
};
