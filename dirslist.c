#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "dirslist.h"

#define uint unsigned int

/*
 *
 */

// Структура хранящая список содержимого каталога в виде
// массива строк (аля argv). Память для записей выделяется
// из кучи.

/*
typedef struct {
    unsigned int count;
    struct item **ilist;
} dirslist_t;
 */

/// Сортирует список файлов поднимая каталоги вверх.
/// \param dl

/*
static void items_list_sort(dirslist_t *dl) {

    struct item *tmp_item = NULL;
    //unsigned int items_count = dl->count;

    for (uint i = 2; i < dl->count; i++) {
        if (*(dl->ilist + i)->itype == ISFILE) {
            tmp_item = *(dl->ilist + (i + 1));
            //dl->ilist
        }
    }

}
*/


/// Очищает кучу по адресам из принятой структуры.
/// \param dl

void items_list_free(dirslist_t **dl) {

    // Чтобы не освободить несуществующий адрес.
    (*dl)->count--;

    while ((*dl)->count) {
        free((*((*dl)->ilist + (*dl)->count))->name);
        free(*((*dl)->ilist + (*dl)->count));
        (*dl)->count--;

        if (!(*dl)->count) {
            free((*((*dl)->ilist))->name);
            free(*((*dl)->ilist)); // Valgrind - Must have!
            free((*dl)->ilist);
        }
    }
    free(*dl);

}

/// Очищает кучу по адресам из принятой структуры.
/// Написана проще предыдущей. Наверное...
/// \param dl

void items_list_free2(dirslist_t **dirl) {

    dirslist_t *dl = *dirl;
    // Чтобы не освободить несуществующий адрес.
    dl->count--;

    while (dl->count) {
        free((*(dl->ilist + dl->count))->name);
        free(*(dl->ilist + dl->count));
        dl->count--;

        if (!dl->count) {
            free((*(dl->ilist))->name);
            free(*(dl->ilist)); // Valgrind - Must have!
            free(dl->ilist);
        }
    }
    free(*dirl);

}



/// Создает в куче структуру типа item, заполняет её
/// принятыми данными и возвращет указатель на нее.  
/// \param item_name - имя
/// \param iamtype - тип (ISDIR или ISFILE)
/// \return 

static struct item *new_item(const char* item_name, enum item_type iamtype) {

    struct item *iitem = malloc(sizeof (struct item));
    iitem->name = malloc(strlen(item_name) + 1);
    strcpy(iitem->name, item_name);
    iitem->itype = iamtype;
    return iitem;
}

/// Возвращает указатель на структуру dirslist_t заполненную новыми данными. 
/// \param dl
/// \param dst_name
/// \return

dirslist_t *items_list(char *dst_name) {

    dirslist_t *dl = malloc(sizeof (dirslist_t));

    // Создаем новые записи. Сразу закинем в начало списка
    // обозначения дочернего и родительского каталогов.

    // Выделяем место под три указателя на структуры.
    dl->ilist = (struct item**) malloc(sizeof (struct item*) * 3);
    // Выделяем место под первую структуру
    // и указатель на нее закидываем в массив
    *(dl->ilist) = malloc(sizeof (struct item));
    // Переходим в нашу структуру, выделяем место 
    // под имя хранимого каталога.
    (*(dl->ilist))->name = malloc(sizeof (char) * sizeof ("."));
    // Копируем имя в структуру.
    strcpy((*(dl->ilist))->name, ".");
    // Указываем что это каталог.
    (*(dl->ilist))->itype = ISDIR;

    // Тоже самое.
    *(dl->ilist + 1) = new_item("..", ISDIR);
    /*
        (*(dl->ilist + 1)) = malloc(sizeof (struct item));
        (*(dl->ilist + 1))->name = malloc(sizeof (char) * sizeof (".."));
        strcpy((*(dl->ilist + 1))->name, "..");
        (*(dl->ilist + 1))->itype = item_type.ISDIR;
     */
    // Указваем что в списке уже есть два элемента.
    dl->count = 2;


    // Структура описатель "потока" каталога.
    DIR *DIRp;
    // Структура описатель текущего каталога.
    struct dirent *current_dir;
    // Тут храним тип записи.
    enum item_type ietype;
    // Содержит инфу о текущем файле.
    struct stat dstat;
    // Открываем каталог.
    DIRp = opendir(dst_name);
    // Если вернулся не NULL начинаем обходить записи данного каталога.
    if (DIRp) {
        // Повторяем пока не NULL т.е. пока не закончились записи.
        while ((current_dir = readdir(DIRp)) != NULL) {

            if (!strcmp(current_dir->d_name, "."))
                continue;
            if (!strcmp(current_dir->d_name, ".."))
                continue;

            if (stat(current_dir->d_name, &dstat) == -1)
                printf("Stat() error!\n");

            if (S_ISDIR(dstat.st_mode))
                ietype = ISDIR;
            else
                ietype = ISFILE;

            *(dl->ilist + dl->count) = new_item(current_dir->d_name, ietype);


            /*
                        // Выделяем место под новую структуру.
             *(dl->ilist + dl->count) = malloc(sizeof (struct item));
                        // Выделяем место под название каталога.
                        (*(dl->ilist + dl->count))->name = malloc(strlen(current_dir->d_name) + 1);
                        // Копируем имя в список.
                        strcpy((*(dl->ilist + dl->count))->name, current_dir->d_name);
             */

            // Увеличиваем список.
            dl->count++;
            dl->ilist = realloc(dl->ilist, sizeof (struct item*) * (dl->count + 1));

        }
        *(dl->ilist + dl->count) = NULL;
        closedir(DIRp);
        return dl;
    } else {
        printf("Return: NULL!\n");
        return NULL;
    }

}

