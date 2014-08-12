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
