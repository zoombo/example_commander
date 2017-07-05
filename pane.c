#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "pane.h"
#include "dirslist.h"

pane_window *new_pane(int height, int width, int start_y, int start_x) {

    pane_window *tmp_pane = malloc(sizeof (pane_window));

    tmp_pane->current_directory = getcwd(NULL, 255);
    tmp_pane->dirlist = items_list(".");

    tmp_pane->height = height;
    tmp_pane->width = width;
    tmp_pane->start_y = start_y;
    tmp_pane->start_x = start_x;

    tmp_pane->position = 0;
    tmp_pane->real_position = 0;

    tmp_pane->is_active = FALSE;

    tmp_pane->PANE_WINDOW = newwin(height, width, start_y, start_x);
    
    
    //box(tmp_pane->pane_window, 0, 0);
    
    //wborder(tmp_pane->pane_window, '|', '|', '-', '-', '+', '+', '+', '+');
    
    //wrefresh(tmp_pane->pane_window); 

    return tmp_pane;

}

