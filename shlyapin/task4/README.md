<h1 align="center">Рассчет определённого интеграла какой-нибудь простой, интегрируемой аналитически на некотором промежутке функции f(x) методом Монте-Карло</h1>


### Параметры, установленные в системе, где производились все замеры

1) Intel Core i3 1215U - 6 ядер и 8 потоков

### Конфигурация программы(можно изменить в файле include/config.h)

1) Количество точек 1 000 000

![](https://github.com/isshlyapin/MIPT-CompTech-2ndCourse/blob/main/task4/image/chart.svg)

![](https://github.com/isshlyapin/MIPT-CompTech-2ndCourse/blob/main/task4/image/grafic1.svg)

![](https://github.com/isshlyapin/MIPT-CompTech-2ndCourse/blob/main/task4/image/grafic2.svg)

### Оценка результатов
Увеличение производительности от увеличения количества используемых потоков до n равному количеству физических ядер довольно существенно(получили результаты отличающиеся в ~5 раз). Стагнация роста производительности наблюдается на промежутке 15 < n < 110
### Запуск на своей машине

1) Клонируйте репозиторий
```
git clone https://github.com/isshlyapin/MIPT-CompTech-2ndCourse.git
```

2) Перейдите в директорию проекта
```
cd MIPT-CompTech-2ndCourse/task4
```

3) Для сборки проекта нам необходима утилита CMake
```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
```
```
cmake --build build
```

1) Для запуска 
```
./bin/integral
```
