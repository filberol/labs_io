# Лабораторная работа 1

**Название:** "Разработка драйверов символьных устройств"

**Цель работы:** "Получить знания и навыки разработки драйверов символьных устройств для операционной системы Linux."

## Описание функциональности драйвера

При записи в файл символьного устройства текста типа “5+6” должен запоминаться результат операции, то есть 11 для
данного примера. Должны поддерживаться операции сложения, вычитания, умножения и деления. Последовательность полученных
результатов с момента загрузки модуля ядра должна выводиться при чтении созданного файла /dev/var2 в консоль
пользователя. При чтении из файла символьного устройства в кольцевой буфер ядра должен осуществляться вывод тех же
данных, которые выводятся при чтении файла /dev/var2.

## Инструкция по сборке

Сборка:

`make build`

Сборка и установка:

`make`

Выгрузка модуля:

`make uninstall`

## Инструкция пользователя

Передача строки в драйвер:

`echo "2 + 2" > /dev/var2`

Чтение результата:

`cat /dev/var2`

## Примеры использования

```user@user-VirtualBox:~/CLionProjects/labs_io/lab1$ echo "3 - 4" > /dev/var2
user@user-VirtualBox:~/CLionProjects/labs_io/lab1$ echo "3 * 4" > /dev/var2
user@user-VirtualBox:~/CLionProjects/labs_io/lab1$ echo "3 / 3" > /dev/var2
user@user-VirtualBox:~/CLionProjects/labs_io/lab1$ echo "3 + 3" > /dev/var2
user@user-VirtualBox:~/CLionProjects/labs_io/lab1$ cat /dev/var2
Result 4: 6
Result 3: 1
Result 2: 12
Result 1: -1
user@user-VirtualBox:~/CLionProjects/labs_io/lab1$ dmesg | tail -n 12
[ 3597.512701] Loading module
[ 3597.512706] drv_module: Registered with major 240
[ 3597.512724] drv_module: Registered device class: drv_modules
[ 3597.516891] drv_module: Registered device: /dev/var2
[ 3645.370328] drv_module: Result 4: 6
[ 3645.370330] drv_module: Result 3: 1
[ 3645.370331] drv_module: Result 2: 12
[ 3645.370331] drv_module: Result 1: -1
[ 3645.370343] drv_module: Result 4: 6
[ 3645.370344] drv_module: Result 3: 1
[ 3645.370345] drv_module: Result 2: 12
[ 3645.370345] drv_module: Result 1: -1
```
