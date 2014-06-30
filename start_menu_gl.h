#ifndef START_H
#define START_H

#include "cursor_gl.h"
#include "image_gl.h"

typedef struct start_menu_t {
    image_t *start_state;
    cursor_t *cursor_state;
} start_menu_t;

void exit_start_menu(start_menu_t *state);
void init_start_menu(start_menu_t *state, gl_t *gl_state);
void render_start_menu(start_menu_t *state, float cursor_x, float cursor_y);

#endif             
