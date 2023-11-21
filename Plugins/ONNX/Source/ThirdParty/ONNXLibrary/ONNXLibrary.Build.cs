using System.IO;
using UnrealBuildTool;

public class ONNXLibrary : ModuleRules
{
    public ONNXLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Win64", "onnxruntime.lib"));
            RuntimeDependencies.Add("$(PluginDir)/Binaries/Win64/onnxruntime.dll");
        }
    }
}
