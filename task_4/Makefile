CC = gcc
CFLAGS = -Wall -O3
LDFLAGS = -pthread -lrt

# Имена исполняемых файлов
INTEGRAL = integral
RESULT = result

# Параметры по умолчанию
THREADS ?= 4
MIN ?= 1
MAX ?= 100
POINTS ?= 1000000

all: $(INTEGRAL) $(RESULT)

$(INTEGRAL): integral.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(RESULT): result.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Запрос параметров у пользователя и запуск
run: $(INTEGRAL) $(RESULT)
	@echo "Enter parameters:"
	@read -p "Number of threads: " threads; \
	read -p "Min value: " min; \
	read -p "Max value: " max; \
	read -p "Number of points: " points; \
	./$(INTEGRAL) $$threads $$min $$max $$points & \
	sleep 1 && ./$(RESULT) $$threads

clean:
	rm -f $(INTEGRAL) $(RESULT)
	rm -f /dev/shm/integral_shm

.PHONY: all run clean
