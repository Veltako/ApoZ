#pragma once

#include "CoreMinimal.h"

// Déclaration unique de la catégorie de log
DECLARE_LOG_CATEGORY_EXTERN(LogApoZ, Log, All);

// Macros de logging spécialisées
#if AZ_CORE_DEBUG
#define AZ_LOG(Verbosity, Format, ...) \
UE_LOG(LogApoZ, Verbosity, TEXT("[ApoZ] " Format), ##__VA_ARGS__)
    
#define AZ_LOG_SCREEN(Format, ...) \
if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, \
FString::Printf(TEXT("[ApoZ] " Format), ##__VA_ARGS__))
    
#define AZ_LOG_FUNC() \
AZ_LOG(VeryVerbose, "Function: %s", ANSI_TO_TCHAR(__FUNCTION__))
        
#define AZ_LOG_COMPONENT(Component, Format, ...) \
AZ_LOG(Log, "[%s] " Format, *Component->GetName(), ##__VA_ARGS__)
        
#define AZ_LOG_ACTOR(Actor, Format, ...) \
AZ_LOG(Log, "[%s] " Format, Actor ? *Actor->GetName() : TEXT("NULL"), ##__VA_ARGS__)
        
#define AZ_LOG_WARN(Format, ...) \
AZ_LOG(Warning, Format, ##__VA_ARGS__)
        
#define AZ_LOG_ERROR(Format, ...) \
AZ_LOG(Error, Format, ##__VA_ARGS__)
#else
#define AZ_LOG(Verbosity, Format, ...)
#define AZ_LOG_SCREEN(Format, ...)
#define AZ_LOG_FUNC()
#define AZ_LOG_COMPONENT(Component, Format, ...)
#define AZ_LOG_ACTOR(Actor, Format, ...)
#define AZ_LOG_WARN(Format, ...)
#define AZ_LOG_ERROR(Format, ...)
#endif

// Macros de vérification
#define AZ_CHECK_VALID(Ptr, ReturnValue) \
if (!IsValid(Ptr)) \
{ \
AZ_LOG_ERROR("Invalid pointer: %s in %s", #Ptr, ANSI_TO_TCHAR(__FUNCTION__)); \
return ReturnValue; \
}

#define AZ_CHECK_VALID_VOID(Ptr) \
if (!IsValid(Ptr)) \
{ \
AZ_LOG_ERROR("Invalid pointer: %s in %s", #Ptr, ANSI_TO_TCHAR(__FUNCTION__)); \
return; \
}