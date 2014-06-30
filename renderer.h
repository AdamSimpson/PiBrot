#ifndef mandel_renderer_h
#define mandel_renderer_h

#include "stdbool.h"

struct exit_menu_t;

// enum of displayed parameter values
typedef enum {
    MIN = 0,
    GRAVITY = MIN,
    VISCOSITY,
    DENSITY,
    PRESSURE,
    ELASTICITY,
    MAX = ELASTICITY
 } parameters;

typedef struct render_t {
    float screen_width;
    float screen_height;
    bool pause;
    bool quit_mode;
    struct exit_menu_t *exit_menu_state;
    int return_value;
    bool start;
    float mouse_x;
    float mouse_y;
} render_t;

#endif
