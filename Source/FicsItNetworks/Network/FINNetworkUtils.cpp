﻿#include "FINNetworkUtils.h"

#include "FINAdvancedNetworkConnectionComponent.h"
#include "FINNetworkComponent.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"

UObject* UFINNetworkUtils::FindNetworkComponentFromObject(UObject* Obj) {
	if (Obj->Implements<UFINNetworkComponent>()) return Obj;
	if (Obj->IsA<AActor>()) {
		TArray<UActorComponent*> Connectors = Cast<AActor>(Obj)->GetComponentsByClass(UFINAdvancedNetworkConnectionComponent::StaticClass());
		for (UActorComponent* Connector : Connectors) {
			if (Connector->Implements<UFINNetworkComponent>()) return Connector;
		}
	}
	return nullptr;
}

FFINNetworkTrace UFINNetworkUtils::RedirectIfPossible(const FFINNetworkTrace& Trace) {
	UObject* Obj = Trace.GetUnderlyingPtr().Get();
	if (Obj && Obj->Implements<UFINNetworkComponent>()) {
		UObject* RedirectObj = IFINNetworkComponent::Execute_GetInstanceRedirect(Obj);
		if (RedirectObj) return Trace / RedirectObj;
	}
	return Trace;
}
