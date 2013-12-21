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
    int rows = 8;

    work->start_row = *rowsTaken;
    int num_rows = (*rowsTaken)+rows<info->num_rows?rows:info->num_rows-(*rowsTaken);
    work->num_rows = num_rows;

    *rowsTaken += num_rows;
}

int get_max_work_size(const FRAC_INFO *info)
{
    return 8*info->num_cols;
}

void pack_and_send(WORK_DATA *work, char *pack_buffer, int empty_size)
{
    int position;
        
    //pack and send work       
    position = 0;
    MPI_Pack(&work->start_row,1,MPI_INT,pack_buffer,empty_size,&position,MPI_COMM_WORLD);
    MPI_Pack(&work->num_rows,1,MPI_INT,pack_buffer,empty_size,&position,MPI_COMM_WORLD);
    MPI_Send(pack_buffer, position, MPI_PACKED, work->rank, WORKTAG, MPI_COMM_WORLD);
}

void recv_and_unpack(WORK_DATA *work, FRAC_INFO *info, char *pack_buffer, int full_size)
{
    int position, msg_size;
    MPI_Status status;

    // Recieve and unpack work
    MPI_Recv(pack_buffer, full_size, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    position = 0;
    work->rank = status.MPI_SOURCE;
    MPI_Get_count(&status, MPI_PACKED, &msg_size);
    MPI_Unpack(pack_buffer, msg_size, &position, &work->start_row,1,MPI_INT,MPI_COMM_WORLD);
    MPI_Unpack(pack_buffer, msg_size, &position, &work->num_rows,1,MPI_INT,MPI_COMM_WORLD);
    MPI_Unpack(pack_buffer, msg_size, &position, work->pixels, work->num_rows*info->num_cols, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
}

void master(const FRAC_INFO *frac_info, const STATE_T *ogl_state)
{
    int ntasks, dest;
    WORK_DATA work_send;
    WORK_DATA work_recv;
    int rowsTaken = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);    

    // Allocate buffer to hold received pixel data
    size_t size = sizeof(unsigned char) * (unsigned long)frac_info->num_cols * (unsigned long)get_max_work_size(frac_info);
    work_recv.pixels = (unsigned char*)malloc(size);
    if(!work_recv.pixels) {
        printf("row pixel buffer allocation failed, %lu bytes\n", size);
        exit(1);
    }

    // Allocate pack buffer
    int member_size, empty_size, full_size;
    int position;
    char *buffer;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &member_size);
    empty_size = member_size;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &member_size);
    empty_size += member_size;
    MPI_Pack_size(get_max_work_size(frac_info), MPI_UNSIGNED_CHAR, MPI_COMM_WORLD, &member_size);
    full_size = empty_size + member_size;

    buffer = malloc(full_size);    
    if(!buffer) {
        printf("buffer allocation failed, %d bytes\n",full_size);
        exit(1);
    }

    // Send initial data
    for (dest = 1; dest < ntasks; dest++) {
        //Get next work item
        get_work(frac_info,&rowsTaken,&work_send);

        // Set destination to send work to
        work_send.rank = dest;

        // Pack work and send
        pack_and_send(&work_send, buffer, empty_size);        
    }

    printf("sent initial work\n");
    //Get next work item
    get_work(frac_info, &rowsTaken, &work_send);

    while(work_send.num_rows) {

	// Receive work load and unpack
        recv_and_unpack(&work_recv, frac_info, buffer, full_size);

        // Update texture with recieved buffer
        update_fractal_rows(ogl_state, 0, work_recv.start_row, work_recv.num_rows, work_recv.pixels);

        // Send more work to the rank we just received from
        work_send.rank = work_recv.rank;

        //pack and send work       
        pack_and_send(&work_send, buffer, empty_size);        

        //Get next work item
        get_work(frac_info, &rowsTaken, &work_send);

    }

    // Recieve all remaining work
    for (dest = 1; dest < ntasks; dest++) {

	// Receive work load and unpack
        recv_and_unpack(&work_recv, frac_info, buffer, full_size);

        // Update texture with received buffer
	update_fractal_rows(ogl_state, 0,  work_recv.start_row, work_recv.num_rows, work_recv.pixels);

        // Kill slaves
        MPI_Send(0,0,MPI_INT,dest,DIETAG,MPI_COMM_WORLD);
    }

    free(buffer);
    free(work_recv.pixels);
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
