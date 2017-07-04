#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#include "dirslist.h"
#include "pane.h"

/*
 * 
 */

#define uint unsigned int

#define TOP_BORDER 5
#define BOTTOM_BORDER 5

// Перерисовываем окошко-панель.
#define REDRAW(pane, pair_color_number) wattron(pane->PANE_WINDOW, COLOR_PAIR(pair_color_number));\
                     box(pane->PANE_WINDOW, 1, 1);\
/*
                     wborder(left_pane->pane_window, '|', '|', '-', '-', '+', '+', '+', '+'); 
*/\
                     wattroff(pane->PANE_WINDOW, COLOR_PAIR(pair_color_number));

int main(int argc, char** argv) {

    initscr();
    refresh();


    // Создаем 2 панели. ///////////////
    // Задаем параметры отдельными переменными т.к. так понятнее.
    int height = getmaxy(stdscr);
    int width = getmaxx(stdscr) / 2 - 1;
    int start_x = 0;
    int start_y = 0;

    pane_window *left_pane = new_pane(height, width, start_y, start_x);

    // Еще раз только поменяем стартовую позицию по X.
    height = getmaxy(stdscr);
    width = getmaxx(stdscr) / 2 - 0;
    start_x = getmaxx(stdscr) / 2 + 0;
    start_y = 0;

    pane_window *right_pane = new_pane(height, width, start_y, start_x);

    // Отрисовываем окошки-панели.
    /*
        wrefresh(left_pane->PANE_WINDOW);
        wrefresh(right_pane->PANE_WINDOW);
     */
    ////////////////////////////////////

    ////////////////////////////////////
    // Содержит код нажатой клавиши.
    unsigned int pressed_key = 0;

    // Не выводить нажатые клавиши
    noecho();
    // Цветные рамка и выделение.
    start_color();

    // Цвет курсора.
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    // Цвет рамки.
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    // Цвет progress bar.
    init_pair(3, COLOR_BLACK, COLOR_MAGENTA);

    // Управление клавиатурой.
    keypad(stdscr, true);

    // Указатели на активную и неактивную панели.
    pane_window *active_pane = left_pane;
    pane_window *INactive_pane = right_pane;

    ////////////////////////////////////

    // Отрисовываем на старте.
    REDRAW(left_pane, 2);
    REDRAW(right_pane, 2);
    wrefresh(left_pane->PANE_WINDOW);
    wrefresh(right_pane->PANE_WINDOW);

    /*
        // Получаем списки файлов для каждой панели.
        active_pane->dirlist = items_list(".");
        INactive_pane->dirlist = items_list(".");
     */

    // int n = 0;
    while (true) {

        //REDRAW(active_pane);

        // Если нажата 'q' выходим.
        if (pressed_key == 'q')
            break;

        // Обмен указателей на активную и неактивную панели.
        if (pressed_key == '\t') {
            pane_window *tmp_pane_ptr = active_pane;
            active_pane = INactive_pane;
            INactive_pane = tmp_pane_ptr;

            // Переход в каталог активной панели.
            chdir(active_pane->current_directory);
        }

        /*
         * Тут странная логика которая сделана для правильного 
         * перемещения курсора по списку внутри окна.
         * real_position - позиция указывающая на элемент списка.
         * position - позиция списка выводимого в окно.
         * BOTTOM_BORDER - ширина рамки.
         * Положение position зависит от real_position так:
         *   если real_position становится больше position, значит курсор
         *   уходит за нижний край, значит увеличиваем position;
         *   
         *   если real_position становится меньше position, значит курсор
         *   уходит за верхний край, значит уменьшаем position.
         */
        if (pressed_key == KEY_UP) {
            if (active_pane->real_position > 0)
                active_pane->real_position--;
            if (active_pane->real_position < (active_pane->position + BOTTOM_BORDER) - getmaxy(stdscr))
                if (active_pane->position > 0)
                    active_pane->position--;
        }
        if (pressed_key == KEY_DOWN) {
            if (active_pane->position < active_pane->dirlist->count - 1 || active_pane->real_position < active_pane->position) {
                active_pane->real_position++;
                if (active_pane->real_position > active_pane->position)
                    active_pane->position++;
            }
        }

        // Если нажат ENTER
        if (pressed_key == '\n') {

            // Тут запуск файла.

            if ((*(active_pane->dirlist->ilist + active_pane->real_position))->itype == ISFILE) {
                char work[255];
                // Тут строим полный путь к запускаемому файлу.
                strcpy(work, getcwd(NULL, 255));
                strcat(work, "/");
                strcat(work, (*(active_pane->dirlist->ilist + active_pane->real_position))->name);

                //printw("\n\n %s\n", work);
                // Временно закрываем окно 
                clear();
                refresh();
                endwin();
                printf("\n%s\n", work);
                pid_t ipid;
                ipid = fork();
                if (ipid == 0) {
                    execl(work, work, NULL);
                    //execl("/usr/bin/ls", "/usr/bin/ls", "-al", NULL);
                    exit(0);
                }
                // Что-то типа лога.
                // FILE *file_ptr = fopen("le3_log.log", "w");
                // fprintf(file_ptr, "\n %s - %s \n", work, "was executed.\n");
                // fclose(file_ptr);

                int statuss;
                wait(&statuss);
                getch();
                pressed_key = ' ';
                // Снова открываем окно.
                initscr();
                continue;
            }



            // Переходим в каталог на котором нажали ENTER
            chdir((*(active_pane->dirlist->ilist + active_pane->real_position))->name);
            // Освобождаем память занятую предыдущим списком.
            items_list_free(&active_pane->dirlist);
            //free(active_pane->current_directory);
            // Загружаем список файлов нового каталога.
            active_pane->dirlist = items_list(".");
            // Обнуляем позицию.
            active_pane->position = 0;
            active_pane->real_position = 0;
            // Обнуляем чтобы не зациклилось.
            pressed_key = ' ';
            continue;
        }

        if (pressed_key == KEY_F(5)) {
            int width = 50;
            WINDOW *progress_bar_win = newwin(6, width, getmaxy(stdscr) / 3,
                    getmaxx(stdscr) / 2 - (width / 2));
            box(progress_bar_win, 0, 0);
            wrefresh(progress_bar_win);

            for (int i = 0; i < 10; i++) {

                wattron(progress_bar_win, COLOR_PAIR(3));
                mvwprintw(progress_bar_win, 3, i + 2, " ");
                wattroff(progress_bar_win, COLOR_PAIR(3));

                wrefresh(progress_bar_win);
                sleep(1);
                
            }

            //sleep(5);

            wborder(progress_bar_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

            wrefresh(progress_bar_win);
            pressed_key = ' ';
            continue;
        }

        active_pane->current_directory = getcwd(NULL, 255);
        // Отображает текущий каталог в заголовке.
        // Valgrind говорит что в этой функции иногда происходят ошибки.
        mvwprintw(active_pane->PANE_WINDOW, 1, 1, " > %s\n", active_pane->current_directory);
        // mvwprintw(INactive_pane->PANE_WINDOW, 1, 1, " > %s\n", INactive_pane->current_directory);

        // Переменная хранящая позицию с которой 
        // начинать выводить список файлов.
        int tmp_pos = 0;
        //if (real_position)
        // Вычисляем номер элемента с которого выводить список если
        // курсор ушел за нижний край.
        if (active_pane->position + TOP_BORDER > getmaxy(stdscr)) {
            tmp_pos = active_pane->position - getmaxy(stdscr);
            tmp_pos += BOTTOM_BORDER;
        }

        /* Этот код работает, это "старый" вариант. Не удаляю потому что пока
         * не знаю, действительно-ли это код сложнее второго варианта.
         * Время покажет.
         
                // Вывод списка.
                for (; tmp_pos < active_pane->dirlist->count; tmp_pos++) {
                    if (tmp_pos == active_pane->real_position) {
                        wattron(active_pane->PANE_WINDOW, COLOR_PAIR(1));

                        if ((*(active_pane->dirlist->ilist + tmp_pos))->itype == ISDIR)
                            wprintw(active_pane->PANE_WINDOW, "\n  /%s", (*(active_pane->dirlist->ilist + tmp_pos))->name);

                        if ((*(active_pane->dirlist->ilist + tmp_pos))->itype == ISFILE)
                            wprintw(active_pane->PANE_WINDOW, "\n  %s", (*(active_pane->dirlist->ilist + tmp_pos))->name);

                        wattroff(active_pane->PANE_WINDOW, COLOR_PAIR(1));
                    } else {
                        if ((*(active_pane->dirlist->ilist + tmp_pos))->itype == ISDIR)
                            wprintw(active_pane->PANE_WINDOW, "\n  /%s", (*(active_pane->dirlist->ilist + tmp_pos))->name);

                        if ((*(active_pane->dirlist->ilist + tmp_pos))->itype == ISFILE)
                            wprintw(active_pane->PANE_WINDOW, "\n  %s", (*(active_pane->dirlist->ilist + tmp_pos))->name);
                    }
                }
         */


        /* Вывод списка.
         * Тут попробуем упростить чтобы не рябило в глазах от указетелей.         
         */

        uint count = active_pane->dirlist->count;
        uint real_position = active_pane->real_position;
        struct item **list_item = active_pane->dirlist->ilist;

        for (; tmp_pos < count; tmp_pos++) {
            if (tmp_pos == real_position) {
                wattron(active_pane->PANE_WINDOW, COLOR_PAIR(1));

                if ((*(list_item + tmp_pos))->itype == ISDIR)
                    wprintw(active_pane->PANE_WINDOW, "\n  /%s", (*(list_item + tmp_pos))->name);

                if ((*(list_item + tmp_pos))->itype == ISFILE)
                    wprintw(active_pane->PANE_WINDOW, "\n  %s", (*(list_item + tmp_pos))->name);

                wattroff(active_pane->PANE_WINDOW, COLOR_PAIR(1));
            } else {
                if ((*(list_item + tmp_pos))->itype == ISDIR)
                    wprintw(active_pane->PANE_WINDOW, "\n  /%s", (*(list_item + tmp_pos))->name);

                if ((*(list_item + tmp_pos))->itype == ISFILE)
                    wprintw(active_pane->PANE_WINDOW, "\n  %s", (*(list_item + tmp_pos))->name);
            }
        }



        REDRAW(active_pane, 2);
        wrefresh(active_pane->PANE_WINDOW);
        //wrefresh(INactive_pane->PANE_WINDOW);

        pressed_key = getch();

        wclear(active_pane->PANE_WINDOW);
        //wclear(INactive_pane->PANE_WINDOW);

    }

    items_list_free(&left_pane->dirlist);
    items_list_free(&right_pane->dirlist);

    free(left_pane);
    free(right_pane);
    refresh();
    endwin();

    return (EXIT_SUCCESS);
}

