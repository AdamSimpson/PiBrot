#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "mpi.h"
#include "multi_tex.h"
#include "fractal.h"

void get_work(const FRAC_INFO  *info, int *rowsTaken, WORK_DATA *work);
int get_max_work_size(const FRAC_INFO *info);
void master(const FRAC_INFO *info, const STATE_T *ogl_state);
void slave(const FRAC_INFO *info);


#endif
