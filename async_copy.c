#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// Структура в которую отображается процесс копирования.

struct copy_status {
    // Общий размер исходного файла.
    unsigned long int size;
    // Размер уже скопированных данных.
    unsigned long int read_size;
    // Статус операции.
    //  0 - ничего не значит (стандартное значение после инициализации).
    //  1 - операция копирования началась.
    //  2 - операция копирования завершилась успешно.
    // -1 - произошла ошибка.
    int status;
};

// Структура в которой передаются параметры функции copy().

struct copy_struct {
    // Имена исходного файла и файла назначения.
    char *src, *dst;
    // Указатель на структуру в которую будет выводиться результат.
    struct copy_status *cps;
};

// Функция копирования собственной персоной.
static void *copy(void *st_ptr);

// Обертка над copy() для pthreads_create().

void async_copy(char *src, char *dst, struct copy_status *cps) {

    // Заполняем прилетевшую извне структуру статуса нулями. На всякий случай.
    memset(cps, 0, sizeof (struct copy_status));

    // Выделяем в куче место под структуру передаваемую в поток.
    struct copy_struct *cp_st = malloc(sizeof (struct copy_struct));
    // Заполняем структуру прилетевшими данными.
    cp_st->src = src;
    cp_st->dst = dst;
    cp_st->cps = cps;

    pthread_t thr;
    pthread_create(&thr, NULL, &copy, (void*) cp_st);

}

static void *copy(void *st_ptr) {

    // Приводим полученный указатель к указателю на структуру copy_struct.
    struct copy_struct *cp_str_ptr = st_ptr;

    // Чуть упрощаем чтобы легче читалось.
    char *src = cp_str_ptr->src;
    char *dst = cp_str_ptr->dst;
    struct copy_status *cps = cp_str_ptr->cps;

    // Дескрипторы открытых файлов (исходного и создаваемой копии).
    int src_fd, dst_fd;
    // Открываем исходный файл.
    src_fd = open(src, O_RDONLY);
    // Если не удалось открыть выдаем ошибку.
    if (src_fd == -1) {
        cps->status = -1;
        // printf("Error open src : %s .\n", src);

    }// Если открылось идем дальше.
    else {
        // Открываем файл назначения. Если он есть, будет ошибка,
        // если нету, он будет создан.
        dst_fd = open(dst, O_WRONLY | O_CREAT | O_EXCL, 0664);
        // Если не удалось создать выдаем ошибку, и закрываем исходный файл. 
        if (dst_fd == -1) {
            // printf("Error create dst.\n");
            close(src_fd);
            cps->status = -1;
        }// Если оба файла открыты, начинаем копирование.
        else {

            //Тут получаем статустику по исходному файлу, достаем
            // оттуда размер и присваеваем его полученной структуре copy_status.
            struct stat src_stat;
            stat(src, &src_stat);
            cps->size = src_stat.st_size;

            // printf("Copying... %s --> %s\n", src, dst);

            // Тут создаем буфер и копируем файлик.
#define BUF_SIZE 4096
            char buf[BUF_SIZE + 1];
            int reads_bytes;
            cps->status = 1;
            int write_status;
            while ((reads_bytes = read(src_fd, buf, BUF_SIZE)) > 0) {
                write_status = write(dst_fd, buf, reads_bytes);
                if (write_status == -1) {
                    cps->status = -1;
                    break;
                }
                cps->read_size += reads_bytes;
            }

            close(src_fd);
            close(dst_fd);
            if (cps->status != -1)
                cps->status = 2;
        }
    }
    // Освобождаем место выделенное в куче в функции async_copy().
    free(st_ptr);
}
