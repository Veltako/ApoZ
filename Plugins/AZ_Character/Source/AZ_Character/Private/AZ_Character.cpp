#include "AZ_Character.h"
#include "AZ_Core/Public/Utils/AZLogger.h"

DEFINE_LOG_CATEGORY(LogAZCharacter);

#define LOCTEXT_NAMESPACE "FAZ_CharacterModule"

void FAZ_CharacterModule::StartupModule()
{
	AZ_LOG(Warning, "AZ_Character module loaded successfully!");
}

void FAZ_CharacterModule::ShutdownModule()
{
    AZ_LOG(Warning, "AZ_Character module shutting down");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAZ_CharacterModule, AZ_Character)