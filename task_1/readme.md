# Эмулятор консоли

## Описание
Данная программа представляет собой полноценный эмулятор консоли, который может запускать отдельные команды, группы команд последовательно или конвейеры из команд.

## Скачивание и установка

Скачать и установить эмулятор консоли можно с официальной страницы на github(https://github.com/dmitry131131/console_emulator.git)

Выполните следующие команды:

``` bash
    git clone https://github.com/dmitry131131/console_emulator.git
```

Затем перейдите в директорию, в которую был клонирован репозиторий и выполните:

``` bash
    make
```

После в корне проекта появится исполняемый файл **console**

## Использование

Запустите console и наслаждайтесь использованием.

### Примеры команд

Одиночная команда:

``` bash
    /home/cybermeatball/Files/Efanov_projects/console_emulator >> ls
    build  console  include  Makefile  readme.md  src
```

Несколько команд:

``` bash
    /home/cybermeatball/Files/Efanov_projects/console_emulator >> ls && lsblk
    build  console  include  Makefile  readme.md  src
    NAME        MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
    zram0       254:0    0   3.6G  0 disk [SWAP]
    nvme0n1     259:0    0 476.9G  0 disk 
    ├─nvme0n1p1 259:1    0     1G  0 part /boot
    └─nvme0n1p2 259:2    0 475.9G  0 part /
```

Конвейер из команд:

``` bash
    /home/cybermeatball/Files/Efanov_projects/console_emulator >> ls | grep read | wc
      1       1      10
```