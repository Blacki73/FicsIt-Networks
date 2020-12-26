﻿#pragma once

#include "FINReflectionUIStyle.h"
#include "SlateBasics.h"

class FFINReflectionUIContext;
class UFINSignal;
class UFINClass;
class UFINStruct;
class UFINFunction;
class UFINProperty;

TSharedRef<SWidget> GenerateDataTypeIcon(UFINProperty* Prop, FFINReflectionUIContext* Context);
TSharedRef<SWidget> GeneratePropTypeIcon(UFINProperty* Prop, FFINReflectionUIContext* Context);
TSharedRef<SWidget> GenerateFuncTypeIcon(UFINFunction* Func, FFINReflectionUIContext* Context);

class FFINReflectionUIEntry;

DECLARE_MULTICAST_DELEGATE_OneParam(FFINReflectionUISelectionChanged, FFINReflectionUIEntry*)

class FFINReflectionUIFilter {
private:
	TArray<FString> Tokens;	
public:
	FFINReflectionUIFilter(FString Filter);
	
	bool PassesFilter(const FString& String) const;
};

class FFINReflectionUIEntry : public TSharedFromThis<FFINReflectionUIEntry> {
public:
	FFINReflectionUIContext* Context;

	FFINReflectionUIEntry(FFINReflectionUIContext* Context) : Context(Context) {}
	virtual ~FFINReflectionUIEntry() = default;
	
	virtual TArray<TSharedPtr<FFINReflectionUIEntry>> GetChildren() { return TArray<TSharedPtr<FFINReflectionUIEntry>>(); }
	virtual TSharedRef<SWidget> GetDetailsWidget() = 0;
	virtual TSharedRef<SWidget> GetShortPreview() = 0;
	virtual TSharedRef<SWidget> GetPreview() = 0;
	virtual bool ApplyFilter(const FFINReflectionUIFilter& Filter) = 0;
};

class FFINReflectionUIStruct : public FFINReflectionUIEntry {
protected:
	UFINStruct* Struct;
	TArray<TSharedPtr<FFINReflectionUIEntry>> Filtered;

public:
	TArray<TSharedPtr<FFINReflectionUIEntry>> Attributes;
	TArray<TSharedPtr<FFINReflectionUIEntry>> Functions;
	
	FFINReflectionUIStruct(UFINStruct* Struct, FFINReflectionUIContext* Context);
	
	virtual TArray<TSharedPtr<FFINReflectionUIEntry>> GetChildren() override { return Filtered; }
	virtual TSharedRef<SWidget> GetDetailsWidget() override;
	virtual TSharedRef<SWidget> GetShortPreview() override;
	virtual TSharedRef<SWidget> GetPreview() override;
	virtual bool ApplyFilter(const FFINReflectionUIFilter& Filter) override;

	UFINStruct* GetStruct() const { return Struct; }
};

class FFINReflectionUIClass : public FFINReflectionUIStruct {
public:
	TArray<TSharedPtr<FFINReflectionUIEntry>> Signals;
	
	FFINReflectionUIClass(UFINClass* Class, FFINReflectionUIContext* Context);
	
	virtual TSharedRef<SWidget> GetDetailsWidget() override;
	virtual bool ApplyFilter(const FFINReflectionUIFilter& Filter) override;

	UFINClass* GetClass() const;
};

class FFINReflectionUIProperty : public FFINReflectionUIEntry {
private:
	UFINProperty* Property;

public:
	FFINReflectionUIProperty(UFINProperty* Property, FFINReflectionUIContext* Context) : FFINReflectionUIEntry(Context), Property(Property) {}
	
	virtual TSharedRef<SWidget> GetDetailsWidget() override;
	virtual TSharedRef<SWidget> GetShortPreview() override;
	virtual TSharedRef<SWidget> GetPreview() override;
	virtual bool ApplyFilter(const FFINReflectionUIFilter& Filter) override;
};

class FFINReflectionUIFunction : public FFINReflectionUIEntry {
private:
	UFINFunction* Function;

public:
	FFINReflectionUIFunction(UFINFunction* Function, FFINReflectionUIContext* Context) : FFINReflectionUIEntry(Context), Function(Function) {}

	virtual TSharedRef<SWidget> GetDetailsWidget() override;
	virtual TSharedRef<SWidget> GetShortPreview() override;
	virtual TSharedRef<SWidget> GetPreview() override;
	virtual bool ApplyFilter(const FFINReflectionUIFilter& Filter) override;
};

class FFINReflectionUISignal : public FFINReflectionUIEntry {
private:
	UFINSignal* Signal;

public:
	FFINReflectionUISignal(UFINSignal* Signal, FFINReflectionUIContext* Context) : FFINReflectionUIEntry(Context), Signal(Signal) {}

	virtual TSharedRef<SWidget> GetDetailsWidget() override;
	virtual TSharedRef<SWidget> GetShortPreview() override;
	virtual TSharedRef<SWidget> GetPreview() override;
	virtual bool ApplyFilter(const FFINReflectionUIFilter& Filter) override;
};

class FFINReflectionUIContext : TSharedFromThis<FFINReflectionUIContext> {
private:
	FFINReflectionUIEntry* SelectedEntry = nullptr;
	
public:
	TAttribute<const FFINReflectionUIStyleStruct*> Style;
	TArray<TSharedPtr<FFINReflectionUIEntry>> Entries;
	TMap<UFINStruct*, TSharedPtr<FFINReflectionUIStruct>> Structs;
	FString FilterString;
	
	FFINReflectionUISelectionChanged OnSelectionChanged;

	FFINReflectionUIContext();
	
	void SetSelected(FFINReflectionUIEntry* Entry) {
		if (Entry == SelectedEntry) return;
		SelectedEntry = Entry;
		OnSelectionChanged.Broadcast(SelectedEntry);
	}
	
	FFINReflectionUIEntry* GetSelected() const {
		return SelectedEntry;
	}
};
