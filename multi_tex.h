#ifndef MULTI_TEX_H
#define MULTI_TEX_H

#include "ogl_utils.h"
#include "fractal.h"

#define NUM_TEXTURES 2
#define LEFT 0
#define RIGHT 1

typedef struct texture_t
{
    // OpenGL|ES state
    gl_t *gl_state;

    // Program handle
    GLuint program;

    // Locations
    GLint position_location;
    GLint tex_coord_location;
    GLint tex_location;

    // Texture handles
    GLuint textures[NUM_TEXTURES];

    // Texture attributes
    GLsizei tex_width[NUM_TEXTURES];
    GLsizei tex_height[NUM_TEXTURES];

    GLuint vbo;
    GLuint ebo;

    int terminate;
} texture_t;

void update_texture_rows(texture_t *state, int fractal, GLsizei start_row, GLuint num_rows, GLubyte *row_pixels);
void update_fractal_rows(texture_t *state, int fractal, unsigned int start_row, unsigned int num_rows, unsigned char *row_pixels);
void create_textures(texture_t *state, FRAC_INFO *frac_left, FRAC_INFO *frac_right);
void create_vertices(texture_t *state);
void create_shaders(texture_t *state);
void draw_textures(texture_t *state);
//void show_left_tex_fullscreen(texture_t *state);
//void show_right_tex_fullscreen(texture_t *state);
//void show_both_textures(texture_t *state);

#endif
