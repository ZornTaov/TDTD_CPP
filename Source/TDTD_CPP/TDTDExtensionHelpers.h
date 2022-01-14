#pragma once

template<typename T>
static FString GetEnumName(T InValue)
{
	return StaticEnum<T>()->GetNameStringByIndex(static_cast<int32>(InValue));
}

template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out)
{
	for (TActorIterator<T> It(World); It; ++It)
	{
		Out.Add(*It);
		It->SelectionCursor->SetVisibility(true);
	}
}