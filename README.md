# Технологии параллельного программирования OpenMP и OpenMPI в Linux

## Описание проекта
Выполненная работа ключает:
- Реализацию алгоритмов сжатия (Хаффмана и RLE)
- Сравнение эффективности параллельных вычислений

## Технологии
| Технология | Назначение |
|------------|------------|
| OpenMP | Параллельные вычисления на многоядерных системах |
| OpenMPI | Распределенные вычисления на кластере |
| VirtualBox | Виртуализация для создания тестовой среды |
| Debian 11 | Рабочая ОС для экспериментов |

## Установка и настройка

### 1. Подготовка виртуальных машин
```bash
# Установка VirtualBox
sudo apt install virtualbox

# Создание NAT-сети
VBoxManage natnetwork add --netname NatNetwork --network "10.16.127.64/26" --enable
```

### 2. Настройка Debian
```bash
# Установка базовых компонентов
sudo apt install build-essential git openssh-server

# Настройка SSH
sudo systemctl enable ssh
sudo systemctl start ssh
```
### 3. Установка инструментов параллельного программирования
```bash
# Для OpenMP
sudo apt install libomp-dev

# Для OpenMPI
sudo apt install libopenmpi-dev openmpi-bin
```
## Запуск программы
### Пример с OpenMP
```cpp
#include <omp.h>
#include <stdio.h>

int main() {
    #pragma omp parallel
    {
        printf("Hello from thread %d\n", omp_get_thread_num());
    }
    return 0;
}
```
Компиляция и запуск
```bash
g++ -fopenmp program.cpp -o program
./program
```
### Пример с OpenMPI
```cpp
#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cout << "Hello from process " << rank << std::endl;
    
    MPI_Finalize();
    return 0;
}
```
Компиляция и запуск на 4 процессах:
```bash
mpic++ program.cpp -o program
mpirun -np 4 ./program
```
