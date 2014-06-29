#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "mpi.h"
#include "multi_tex.h"
#include "fractal.h"
#include "renderer.h"

void get_work(FRAC_INFO *info, WORK_DATA *work);
int  get_max_work_size(FRAC_INFO *info);
void master(render_t *render_state, FRAC_INFO *frac_left, FRAC_INFO *frac_right, texture_t *texture_state);
void slave(FRAC_INFO *info);
void master_pack_and_send(WORK_DATA *work, char *pack_buffer, int buffer_size);
int  master_recv_and_unpack(WORK_DATA *work, char *pack_buffer, int buffer_size);
void slave_pack_and_send(WORK_DATA *work, FRAC_INFO *frac_info, char *pack_buffer, int buffer_size);
int  slave_recv_and_unpack(WORK_DATA *work, char *pack_buffer, int buffer_size);

#endif
