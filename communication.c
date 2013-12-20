#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <mpi.h>
#include "fractal.h"
#include "communication.h"

#define WORKTAG 1
#define DIETAG 2

void get_work(const FRAC_INFO  *info, int *rowsTaken, WORK_DATA *work)
{
    if(*rowsTaken >= info->num_rows){
        work->num_rows = 0;
        return;
    }
    int rows = 16;

    work->start_row = *rowsTaken;
    int num_rows = (*rowsTaken)+rows<info->num_rows?rows:info->num_rows-(*rowsTaken);
    work->num_rows = num_rows;

    *rowsTaken += num_rows;
}

int get_max_work_size(const FRAC_INFO *info)
{
    return 16*info->num_cols;
}

void master(const FRAC_INFO *info, const STATE_T *ogl_state)
{
    int ntasks, dest, msgsize;
    WORK_DATA *work = malloc(sizeof(*work));
    MPI_Status status;
    int rowsTaken = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);    

    // Allocate buffer to hold received pixel data
    size_t size = sizeof(unsigned char) * (unsigned long)info->num_cols * (unsigned long)get_max_work_size(info);
    unsigned char *row_pixels = (unsigned char*)malloc(size);
    if(!row_pixels) {
        printf("row pixel buffer allocation failed, %lu bytes\n", size);
        exit(1);
    }

    // Allocate buffer
    int membersize, emptysize, fullsize;
    int position;
    char *buffer;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &membersize);
    emptysize = membersize;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &membersize);
    emptysize += membersize;
    MPI_Pack_size(get_max_work_size(info), MPI_UNSIGNED_CHAR, MPI_COMM_WORLD, &membersize);
    fullsize = emptysize + membersize;

    buffer = malloc(fullsize);    
    if(!buffer) {
        printf("buffer allocation failed, %d bytes\n",fullsize);
        exit(1);
    }

    // Send initial data
    for (dest = 1; dest < ntasks; dest++) {
        //Get next work item
        get_work(info,&rowsTaken,work);
        
        //pack and send work       
        position = 0;
        MPI_Pack(&work->start_row,1,MPI_INT,buffer,emptysize,&position,MPI_COMM_WORLD);
        MPI_Pack(&work->num_rows,1,MPI_INT,buffer,emptysize,&position,MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, dest, WORKTAG, MPI_COMM_WORLD);
    }

    printf("sent initial work\n");
    //Get next work item
    get_work(info,&rowsTaken,work);
    int start_row, num_rows;
    while(work->num_rows) {
        // Recieve and unpack work
        MPI_Recv(buffer, fullsize, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        position = 0;
        MPI_Get_count(&status, MPI_PACKED, &msgsize);
        MPI_Unpack(buffer, msgsize, &position, &start_row,1,MPI_INT,MPI_COMM_WORLD);
        MPI_Unpack(buffer, msgsize, &position, &num_rows,1,MPI_INT,MPI_COMM_WORLD);
        // Is this neccessary or can we update fractal directly from buffer?   
        MPI_Unpack(buffer, msgsize, &position, row_pixels, num_rows*info->num_cols, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

        // Update texture with recieved buffer
        update_fractal_rows(ogl_state, 0, start_row, num_rows, row_pixels);

        //pack and send work       
        position = 0;
        MPI_Pack(&work->start_row,1,MPI_INT,buffer,emptysize,&position,MPI_COMM_WORLD);
        MPI_Pack(&work->num_rows,1,MPI_INT,buffer,emptysize,&position,MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, status.MPI_SOURCE, WORKTAG, MPI_COMM_WORLD);

        //Get next work item
        get_work(info,&rowsTaken,work);

        if(status.MPI_SOURCE==1)
            printf("%d\n",work->start_row);
    }

    // Recieve all remaining work
    for (dest = 1; dest < ntasks; dest++) {
        // Recieve and unpack work
        MPI_Recv(buffer, fullsize, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        position = 0;
        MPI_Get_count(&status, MPI_PACKED, &msgsize);

        MPI_Unpack(buffer, msgsize, &position, &start_row,1,MPI_INT,MPI_COMM_WORLD);
        MPI_Unpack(buffer, msgsize, &position, &num_rows,1,MPI_INT,MPI_COMM_WORLD);
        // unpack pixel data
        MPI_Unpack(buffer, msgsize, &position, row_pixels, num_rows*info->num_cols, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

        // Update texture with received buffer
	update_fractal_rows(ogl_state, 0,  start_row, num_rows, row_pixels);

        // Kill slaves
        MPI_Send(0,0,MPI_INT,dest,DIETAG,MPI_COMM_WORLD);
    }

    free(work);
    free(buffer);
    free(row_pixels);
}

void slave(const FRAC_INFO *info)
{
    MPI_Status status;
    int msgsize;
    WORK_DATA *data = malloc(sizeof(*data));
    data->pixels = (unsigned char*)malloc(get_max_work_size(info)*sizeof(unsigned char));  

    // Allocate buffers
    int membersize, emptysize, fullsize;
    int position;
    char *buffer; //Contains no pixel data
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &membersize);
    emptysize = membersize;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &membersize);
    emptysize += membersize;
    MPI_Pack_size(get_max_work_size(info), MPI_UNSIGNED_CHAR, MPI_COMM_WORLD, &membersize);
    fullsize = emptysize+membersize;
    buffer = malloc(fullsize);

    while(1) {
        // Recieve and unpack work
        MPI_Recv(buffer, emptysize, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // Check tag for work/die
        if(status.MPI_TAG == DIETAG) {
            return;
        }

        // Unpack work info
        position = 0;
        MPI_Get_count(&status, MPI_PACKED, &msgsize);
        MPI_Unpack(buffer, msgsize, &position, &data->start_row,1,MPI_INT,MPI_COMM_WORLD);
        MPI_Unpack(buffer, msgsize, &position, &data->num_rows,1,MPI_INT,MPI_COMM_WORLD);

        // calcPixels
        calcPixels(info, data);        

        // Pack and send data back
        position = 0;
        MPI_Pack(&data->start_row,1,MPI_INT,buffer,fullsize,&position,MPI_COMM_WORLD);
        MPI_Pack(&data->num_rows,1,MPI_INT,buffer,fullsize,&position,MPI_COMM_WORLD);
        MPI_Pack(data->pixels, data->num_rows*info->num_cols, MPI_UNSIGNED_CHAR,buffer,fullsize,&position,MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, 0, WORKTAG, MPI_COMM_WORLD);
    }
}
