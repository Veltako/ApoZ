#include "AZ_Core.h"

DEFINE_LOG_CATEGORY(LogAZCore);

#define LOCTEXT_NAMESPACE "FAZ_CoreModule"

void FAZ_CoreModule::StartupModule()
{
    UE_LOG(LogAZCore, Warning, TEXT("AZ_Core module loaded successfully!"));
}

void FAZ_CoreModule::ShutdownModule()
{
    // Ce code s'exécute quand le module se décharge
    UE_LOG(LogAZCore, Warning, TEXT("AZ_Core module shutting down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAZ_CoreModule, AZ_Core)
