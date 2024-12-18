import os
import subprocess
import time
import hashlib
import matplotlib.pyplot as plt

# Определение размеров буфера
buffer_sizes = ["SMALL", "MEDIUM", "LARGE"]
ipc_methods = ["shm", "fifo", "mqueue"]

# Файлы для проверки данных
input_file = "data/data.dat"
output_file = "data/data.ans"

def get_md5_hash(filename):
    """Возвращает MD5-хеш файла."""
    hasher = hashlib.md5()
    with open(filename, 'rb') as f:
        hasher.update(f.read())
    return hasher.hexdigest()

def run_ipc_method(ipc, buffer_size):
    """Запускает бинарный файл с указанным методом IPC и размером буфера и измеряет время выполнения."""
    print(f"Running {ipc} with {buffer_size} buffer size...")
    start_time = time.time()
    
    # Запускаем бинарный файл и ждём завершения
    process = subprocess.run([f"./{ipc}", buffer_size], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    
    # Вычисляем затраченное время
    elapsed_time = time.time() - start_time
    if process.returncode != 0:
        print(f"Error running {ipc} with {buffer_size}: {process.stderr.decode()}")
        return None
    return elapsed_time

def compare_files(file1, file2):
    """Сравнивает два файла с помощью MD5-хеша."""
    hash1 = get_md5_hash(file1)
    hash2 = get_md5_hash(file2)
    return hash1 == hash2

def create_directory(directory):
    """Создаёт папку, если она не существует."""
    if not os.path.exists(directory):
        os.makedirs(directory)

def main():
    results = {ipc: [] for ipc in ipc_methods}
    
    for ipc in ipc_methods:
        for buffer_size in buffer_sizes:
            # Запуск IPC и измерение времени
            elapsed_time = run_ipc_method(ipc, buffer_size)
            
            if elapsed_time is not None:
                results[ipc].append(elapsed_time)
                
                # Сравнение файлов после передачи
                if compare_files(input_file, output_file):
                    print(f"{ipc} with {buffer_size}: Data integrity check passed!")
                else:
                    print(f"{ipc} with {buffer_size}: Data integrity check FAILED!")
            else:
                results[ipc].append(None)

    # Построение гистограммы
    fig, ax = plt.subplots()
    x = range(len(buffer_sizes))
    width = 0.2

    for i, ipc in enumerate(ipc_methods):
        times = results[ipc]
        ax.bar([p + i * width for p in x], times, width=width, label=ipc)

    ax.set_xlabel("Buffer Size")
    ax.set_ylabel("Time (seconds)")
    ax.set_title("IPC Method Performance Comparison")
    ax.set_xticks([p + width for p in x])
    ax.set_xticklabels(buffer_sizes)
    ax.legend()
    plt.grid(True)

    # Создание папки 'pictures', если её нет
    create_directory("pictures")

    # Сохранение графика в файл
    plt.savefig("pictures/ipc_performance.png")
    print("Histogram saved to 'pictures/ipc_performance.png'")
    plt.show()

if __name__ == "__main__":
    main()

