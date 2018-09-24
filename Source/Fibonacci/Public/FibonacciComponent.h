// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Runtime/Core/Public/HAL/ThreadSafeBool.h"
#include "Runtime/Core/Public/UObject/WeakObjectPtrTemplates.h"
#include "FibonacciComponent.generated.h"

class FFibonacciSlowTask;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIBONACCI_API UFibonacciComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category = "Fibonacci")
	void CalculateFibonacciSequence(int MaxNumber);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fibonacci")
	FText FinalNumber;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFibonacciCompleted);
	UPROPERTY(BlueprintAssignable, Category = "Fibonacci")
	FOnFibonacciCompleted OnFibonacciCompleted;

	UFUNCTION(BlueprintCallable, Category = "Fibonacci")
	void DestroyThreads();

	FFibonacciSlowTask* Task;
};

class FFibonacciSlowTask : public FRunnable
{
public:
	FFibonacciSlowTask(uint64 InMaxIterations, UFibonacciComponent* WeakComponent);
	~FFibonacciSlowTask();
	void EnsureCompletion();
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	uint64 Number;
private:
	TWeakObjectPtr<UFibonacciComponent> WeakComponent;
	uint64 MaxIterations;
	FRunnableThread* Thread;
	FEvent* m_semaphore;
	FThreadSafeBool m_Kill;
};