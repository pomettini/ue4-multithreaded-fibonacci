// Fill out your copyright notice in the Description page of Project Settings.

#include "FibonacciComponent.h"

void UFibonacciComponent::CalculateFibonacciSequence(int MaxNumber)
{
	Task = nullptr;
	Task = new FFibonacciSlowTask(MaxNumber, this);
}

void UFibonacciComponent::DestroyThreads()
{
	if (Task)
	{
		UE_LOG(LogTemp, Warning, TEXT("Distruggo tutto!!!"));
		Task->EnsureCompletion();
		delete Task;
		Task = nullptr;
	}
}

FFibonacciSlowTask::FFibonacciSlowTask(uint64 InMaxIterations, UFibonacciComponent* WeakComponent)
{
	m_Kill = false;
	m_semaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);

	Number = 0;

	this->WeakComponent = WeakComponent;

	MaxIterations = InMaxIterations;
	Thread = FRunnableThread::Create(this, TEXT("Fibonacci thread"));
}

FFibonacciSlowTask::~FFibonacciSlowTask()
{
	if (m_semaphore)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ciao0"));

		//Cleanup the FEvent
		FGenericPlatformProcess::ReturnSynchEventToPool(m_semaphore);
		m_semaphore = nullptr;
	}

	if (Thread)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ciao1"));

		//Cleanup the worker thread
		delete Thread;
		Thread = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Ciao2"));
}

void FFibonacciSlowTask::EnsureCompletion()
{
	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

bool FFibonacciSlowTask::Init()
{
	return true;
}

uint32 FFibonacciSlowTask::Run()
{
	FPlatformProcess::Sleep(0.03);

	//UE_LOG(LogTemp, Warning, TEXT("Starting Number: %llu"), MaxIterations);

	uint64 Prev = 0;
	uint64 Result = 1;
	uint64 Sum;

	for (uint64 i = 0; i < MaxIterations; i++)
	{
		Sum = Result + Prev;
		Prev = Result;
		Result = Sum;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Final Number: %llu"), Result);

	Number = Result;

	FPlatformProcess::Sleep(0.1);
	
	return 0;
}

void FFibonacciSlowTask::Exit()
{
	if (WeakComponent.IsValid())
	{
		WeakComponent.Get()->FinalNumber = FText::AsNumber(Number);
		WeakComponent.Get()->OnFibonacciCompleted.Broadcast();
	}
}