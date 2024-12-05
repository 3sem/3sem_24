#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

// Константы
#define SEGMENT_PERMISSIONS 0666      // Права доступа к разделяемой памяти
#define MAX_RANDOM_SCALE 10000000     // Масштаб для генерации случайных чисел

// Структура для буфера разделяемой памяти
struct MemoryBuffer {
    double resultAccumulator;  // Сумма промежуточных значений интеграла
    sem_t accessSemaphore;     // Семафор для синхронизации доступа
};

#define SHM_SIZE sizeof(struct MemoryBuffer)

// Функция для генерации уникального ключа для разделяемой памяти
key_t generateKey(void);

// Функция для инициализации разделяемой памяти
struct MemoryBuffer* initializeSharedSegment(int* shmID);

// Функция для получения максимальной высоты на отрезке
double GetHeight(double start, double end, double (*func)(double));

// Функция для вычисления количества точек методом Монте-Карло
ssize_t monteCarloEvaluation(double start, double end, double range, double (*func)(double), size_t numSamples);

// Функция для параллельного вычисления интеграла
void executeParallelCalculation(double a, double b, double (*func)(double), int processes, struct MemoryBuffer* sharedMem);

// Основная функция для вычисления определённого интеграла
double computeIntegral(double a, double b, double (*func)(double), int processCount);
