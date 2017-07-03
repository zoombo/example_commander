#include "dirslist.h"

#include <ncurses.h>

#ifndef PANE_H
#define PANE_H


// Структура описывающая окно-панель со списком файлов.

struct pane {
    // Указатель на хендл окна.
    WINDOW *PANE_WINDOW;
    // Указатель на список файлов текущего окна.
    dirslist_t *dirlist;
    // Позиция списка в окне.
    unsigned int position;
    // Позиция выделения в окне.
    unsigned int real_position;

    // Полный путь до текущего каталога.
    char *current_directory;
    // Указывает на то что окно активно в данный момент.
    _Bool is_active;

    // Позиция окна по Y и X.
    unsigned int start_y, start_x;
    // Размер окна по высота и ширина.
    unsigned int height, width;
    //int border_color;
    // Что еще?..
};

typedef struct pane pane_window;

pane_window *new_pane(int height, int width, int start_y, int start_x);

void hide_pane(pane_window *);
void restore_pane(pane_window *);

#endif /* PANE_H */

