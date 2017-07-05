#include <ncurses.h>
#include "dirslist.h"
#include "pane.h"

#define uint unsigned int

/* Вывод списка.
 * Тут попробуем упростить чтобы не рябило в глазах от указетелей.    
 * TODO: Вынести это в отдельную функцию. --> DONE!
 */
void redraw_pane(pane_window *pane, int tmp_pos) {

    uint count = pane->dirlist->count;
    uint real_position = pane->real_position;
    struct item **list_item = pane->dirlist->ilist;

    for (; tmp_pos < count; tmp_pos++) {
        if (tmp_pos == real_position) {
            wattron(pane->PANE_WINDOW, COLOR_PAIR(1));

            if ((*(list_item + tmp_pos))->itype == ISDIR)
                wprintw(pane->PANE_WINDOW, "\n  /%s", (*(list_item + tmp_pos))->name);

            if ((*(list_item + tmp_pos))->itype == ISFILE)
                wprintw(pane->PANE_WINDOW, "\n  %s", (*(list_item + tmp_pos))->name);

            wattroff(pane->PANE_WINDOW, COLOR_PAIR(1));
        } else {
            if ((*(list_item + tmp_pos))->itype == ISDIR)
                wprintw(pane->PANE_WINDOW, "\n  /%s", (*(list_item + tmp_pos))->name);

            if ((*(list_item + tmp_pos))->itype == ISFILE)
                wprintw(pane->PANE_WINDOW, "\n  %s", (*(list_item + tmp_pos))->name);
        }
    }
}