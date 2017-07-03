#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "dirslist.h"
#include "pane.h"

/*
 * 
 */

#define REDRAW(pane) wattron(pane->PANE_WINDOW, COLOR_PAIR(1));\
                     box(pane->PANE_WINDOW, 1, 1);\
/*
                     wborder(left_pane->pane_window, '|', '|', '-', '-', '+', '+', '+', '+'); 
*/\
                     wattroff(pane->PANE_WINDOW, COLOR_PAIR(1));

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
    wrefresh(left_pane->PANE_WINDOW);
    wrefresh(right_pane->PANE_WINDOW);
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

    // Управление клавиатурой.
    keypad(stdscr, true);

    pane_window *active_pane = left_pane;
    pane_window *INactive_pane = right_pane;

    ////////////////////////////////////
    int n = 0;
    while (true) {

        REDRAW(left_pane);
        REDRAW(right_pane);

        /*
        wattron(left_pane->PANE_WINDOW, COLOR_PAIR(1));
        box(left_pane->PANE_WINDOW, 1, 1);
        // wborder(left_pane->pane_window, '|', '|', '-', '-', '+', '+', '+', '+');
        wattroff(left_pane->PANE_WINDOW, COLOR_PAIR(1));

        wattron(right_pane->PANE_WINDOW, COLOR_PAIR(1));
        box(right_pane->PANE_WINDOW, 1, 1);
        // wborder(right_pane->pane_window, '|', '|', '-', '-', '+', '+', '+', '+');
        wattroff(right_pane->PANE_WINDOW, COLOR_PAIR(1));
         */

        if (pressed_key == 'q')
            break;

        // Обмен указателей на активную и неактивную панели.
        if (pressed_key == '\t') {
            pane_window *tmp_pane_ptr = active_pane;
            active_pane = INactive_pane;
            INactive_pane = tmp_pane_ptr;
        }






        if (pressed_key == KEY_UP)
            n--;
        if (pressed_key == KEY_DOWN)
            n++;

        mvwprintw(active_pane->PANE_WINDOW, n, 1, "Hello!\n");
        wattron(active_pane->PANE_WINDOW, COLOR_PAIR(1));
        box(active_pane->PANE_WINDOW, 1, 1);
        wattroff(active_pane->PANE_WINDOW, COLOR_PAIR(1));


        wrefresh(active_pane->PANE_WINDOW);
        wrefresh(INactive_pane->PANE_WINDOW);



        pressed_key = getch();

        wclear(active_pane->PANE_WINDOW);
        wclear(INactive_pane->PANE_WINDOW);

    }
    refresh();
    endwin();

    return (EXIT_SUCCESS);
}

