#include <stdlib.h>
#include <stdio.h>
#include "hdf5.h"

#define NDIMS 2

main(int argc, char** argv)
{
    // Parse command line arguments.
    if (argc != 3) {
        fprintf(stderr, "Usage: simplewrite NY NX\n\n");
        fprintf(stderr, "ERROR: Expected 2 arguments, got %d.\n", argc-1);
        exit(1);
    }
    size_t ny,nx; 
    sscanf(argv[1],"%zu",&ny);
    sscanf(argv[2],"%zu",&nx);

    // Create dimension array.
    hsize_t dims[NDIMS];
    dims[0] = ny;
    dims[1] = nx;

    // Initialize data.
    int iy,ix;
    int* data = (int*) malloc(ny*nx*sizeof(int));
    for (iy=0 ; iy<ny ; iy++) {
        for (ix=0 ; ix<nx ; ix++) {
            data[iy*nx + ix] = 10*iy + ix;
        }
    }

    // Create file name.
    char filename[100];
    sprintf(filename,"simplewrite_%zu_%zu.h5",ny,nx);

    // Write data to HDF5 file.
    hid_t file = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t space = H5Screate_simple(NDIMS,dims,NULL);
    hid_t dset = H5Dcreate(file,"data",H5T_STD_I32LE,space,H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    H5Fclose(file);

    // Free memory.
    free(data);

    // Inform user of the created file.
    printf("%s\n",filename);
}
