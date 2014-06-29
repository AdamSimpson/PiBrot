#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <mpi.h>
#include "fractal.h"
#include "communication.h"
#include "exit_menu_gl.h"

#define WORKTAG 1
#define DIETAG 2

#define MAX_ROWS 1

void get_work(FRAC_INFO  *info, WORK_DATA *work)
{
    if(info->rows_taken >= info->num_rows){
        work->num_rows = 0;
        return;
    }
    int rows = MAX_ROWS;

    work->start_row = info->rows_taken;
    int num_rows = info->rows_taken+rows<info->num_rows?rows:info->num_rows-info->rows_taken;
    work->num_rows = num_rows;

    info->rows_taken += num_rows;

}

int get_max_work_size(FRAC_INFO *info)
{
    return MAX_ROWS*info->num_cols*info->channels;
}

void master_pack_and_send(WORK_DATA *work, char *pack_buffer, int buffer_size)
{
    int position;
        
    //pack and send work       
    position = 0;
    MPI_Pack(&work->start_row,1,MPI_INT,pack_buffer, buffer_size, &position,MPI_COMM_WORLD);
    MPI_Pack(&work->num_rows,1,MPI_INT,pack_buffer, buffer_size, &position,MPI_COMM_WORLD);
    MPI_Send(pack_buffer, position, MPI_PACKED, work->rank, WORKTAG, MPI_COMM_WORLD);
}

int master_recv_and_unpack(WORK_DATA *work, char *pack_buffer, int buffer_size)
{
    int tag, source_rank, position, msg_size, num_pixels;
    FRAC_INFO *frac;
    MPI_Status status;

    // Recieve and unpack work
    MPI_Recv(pack_buffer, buffer_size, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    position = 0;
    work->rank = status.MPI_SOURCE;
 
    // Count of MPI_PACKED type returns bytes
    MPI_Get_count(&status, MPI_PACKED, &msg_size);
    source_rank = status.MPI_SOURCE;

    MPI_Unpack(pack_buffer, msg_size, &position, &work->start_row,1,MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(pack_buffer, msg_size, &position, &work->num_rows,1,MPI_INT, MPI_COMM_WORLD);
    num_pixels = msg_size - position; // Assumes pixel channels are 1 byte each
    MPI_Unpack(pack_buffer, msg_size, &position, work->pixels, num_pixels, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);

    return tag;
}

void slave_pack_and_send(WORK_DATA *work, FRAC_INFO *frac_info, char *pack_buffer, int buffer_size)
{
    int position;
        
    //pack and send work       
    position = 0;
    MPI_Pack(&work->start_row,1,MPI_INT,pack_buffer, buffer_size,&position, MPI_COMM_WORLD);
    MPI_Pack(&work->num_rows,1,MPI_INT,pack_buffer, buffer_size,&position, MPI_COMM_WORLD);
    MPI_Pack(work->pixels, work->num_rows*frac_info->num_cols*frac_info->channels, MPI_UNSIGNED_CHAR, pack_buffer, buffer_size, &position, MPI_COMM_WORLD);
    MPI_Send(pack_buffer, position, MPI_PACKED, 0, WORKTAG, MPI_COMM_WORLD);
}

int slave_recv_and_unpack(WORK_DATA *work, char *pack_buffer, int buffer_size)
{
    int tag, position, msg_size;
    MPI_Status status;

    // Recieve and unpack work
    MPI_Recv(pack_buffer, buffer_size, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
   
    // Check tag for work/die
    tag = status.MPI_TAG;
    if(tag == DIETAG) {
        return tag;
    }

    position = 0;
    MPI_Get_count(&status, MPI_PACKED, &msg_size);
    MPI_Unpack(pack_buffer, msg_size, &position, &work->start_row,1,MPI_INT,MPI_COMM_WORLD);
    MPI_Unpack(pack_buffer, msg_size, &position, &work->num_rows,1,MPI_INT,MPI_COMM_WORLD);

    return tag;
}

void master(render_t *render_state, FRAC_INFO *frac_left, FRAC_INFO *frac_right, texture_t *texture_state)
{
    int ntasks, dest, side;
    WORK_DATA work_send;
    WORK_DATA work_recv;

    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);    

    // Maximum sizes
    int left_size, right_size;
    left_size = get_max_work_size(frac_left);
    right_size = get_max_work_size(frac_right);
    unsigned long max_work_size = left_size > right_size ? left_size : right_size;

    // Allocate buffer to hold received pixel data
    size_t size = sizeof(unsigned char) * max_work_size;
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
    MPI_Pack_size(max_work_size, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD, &member_size);
    full_size = empty_size + member_size;

    buffer = malloc(full_size);    
    if(!buffer) {
        printf("buffer allocation failed, %d bytes\n",full_size);
        exit(1);
    }

    FRAC_INFO *frac;

    // Send initial data
    for (dest = 1; dest < ntasks; dest++) {
       // Rank 1 handles the left fractal
        if(dest==1)
	    frac = frac_left;
	else
	    frac = frac_right;

        //Get next work item
        get_work(frac, &work_send);

        // Set destination to send work to
        work_send.rank = dest;

        // Pack work and send
        master_pack_and_send(&work_send, buffer, empty_size);        
    }

    printf("sent initial work\n");
    int num_ranks_complete = 0;    

    while(num_ranks_complete < ntasks-1) {

        // Render exit menu
        if(render_state->quit_mode)
            render_exit_menu(render_state->exit_menu_state, 0.0, 0.0);

	// Receive work load and unpack
        master_recv_and_unpack(&work_recv, buffer, full_size);

        if(work_recv.rank==1){
	    frac = frac_left;
	    side = LEFT;
	}
	else {
	    frac = frac_right;
	    side = RIGHT;
	}

        // Update texture with recieved buffer
        update_fractal_rows(texture_state, side, work_recv.start_row, work_recv.num_rows, work_recv.pixels);

        // Get more work
        work_send.rank = work_recv.rank;
        get_work(frac, &work_send);

        // Send more work or kill slaves
        if(work_send.num_rows > 0)
            master_pack_and_send(&work_send, buffer, empty_size);        
 	else{
            MPI_Send(0,0,MPI_INT,work_send.rank,DIETAG,MPI_COMM_WORLD);
	    num_ranks_complete++;
        }

    }

    free(buffer);
    free(work_recv.pixels);
}

void slave(FRAC_INFO *frac_info)
{
    MPI_Status status;
    int tag, rank;
    WORK_DATA work;
    work.pixels = (unsigned char*)malloc(get_max_work_size(frac_info)*sizeof(unsigned char));  

    // Allocate buffers
    int member_size, empty_size, full_size;
    int position;
    char *buffer;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &member_size);
    empty_size = member_size;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &member_size);
    empty_size += member_size;
    MPI_Pack_size(get_max_work_size(frac_info), MPI_UNSIGNED_CHAR, MPI_COMM_WORLD, &member_size);
    full_size = empty_size+member_size;
    buffer = malloc(full_size);

    while(1) {
	// Receive work load and unpack
        tag = slave_recv_and_unpack(&work, buffer, empty_size);

        // Check tag for work/die
        if(tag == DIETAG) {
	    free(work.pixels);
            free(buffer);
            return;
        }

        // calcPixels
        if(frac_info->color)
            calcColorPixels(frac_info, &work);        
        else
	    calcPixels(frac_info, &work);
        
        // Pack and send data back
        slave_pack_and_send(&work, frac_info, buffer, full_size);
    }
}
