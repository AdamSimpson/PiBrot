/*
The MIT License (MIT)

Copyright (c) 2014 Adam Simpson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef EXIT_H
#define EXIT_H

#include "cursor_gl.h"
#include "image_gl.h"
#include "rectangle_gl.h"

typedef struct exit_menu_t {
    rectangle_t *rectangle_state;
    image_t *mandelbrot_state;
    image_t *sph_state;
    image_t *terminal_state;
    cursor_t *cursor_state;
} exit_menu_t;

void exit_exit_menu(exit_menu_t *state);
void init_exit_menu(exit_menu_t *state, gl_t *gl_state);
void render_exit_menu(exit_menu_t *state, float cursor_x, float cursor_y);

#endif             
