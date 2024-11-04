import subprocess
import matplotlib.pyplot as plt
import time

def run_experiment(num_threads, total_points):
    # Функция для запуска программ A и B и измерения времени выполнения
    start_time = time.time()
    subprocess.run(["build/compute_integral", str(num_threads), str(total_points)])
    subprocess.run(["build/gather_info", str(num_threads)])
    end_time = time.time()
    return end_time - start_time

def main():
    total_points = 1000000    # Фиксированное число точек
    max_threads = 64          # Максимальное число потоков
    num_trials = 25           # Количество повторений для усреднения

    times = []
    threads = range(1, max_threads + 1)

    for num_threads in threads:
        trial_times = []
        for _ in range(num_trials):
            duration = run_experiment(num_threads, total_points)
            trial_times.append(duration)
        
        avg_time = sum(trial_times) / num_trials
        times.append(avg_time)
        print(f"Threads: {num_threads}, Average Time: {avg_time:.4f} sec")

    plt.plot(threads, times, marker="o")
    plt.xlabel("Number of Threads")
    plt.ylabel("Average Execution Time (s)")
    plt.title("Average Execution Time vs Number of Threads")
    plt.grid()
    plt.show()

if __name__ == "__main__":
    main()

