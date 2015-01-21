#include "h5unixpipe.h"

int
reduce_mult(int ndims, hsize_t* dims) {
    size_t idim = 0;
    size_t res = dims[0];
    for (idim = 1 ; idim < ndims ; idim++) {
        res *= dims[idim];
    }
    return res;
}


void
hdf5_to_data(hid_t fileimage, hsize_t** dims, int *ndims, int** data) {
    hid_t dset = H5Dopen(fileimage,"data",H5P_DEFAULT);
    hid_t space = H5Dget_space(dset);
    *ndims = H5Sget_simple_extent_ndims(space);
    *dims = malloc(*ndims*sizeof(size_t));
    H5Sget_simple_extent_dims(space, *dims, NULL);
    *data = malloc(reduce_mult(*ndims,*dims)*sizeof(int));
    /* todo: check type */
    H5Dread(dset, H5T_STD_I32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT,*data);
    H5Fclose(fileimage);
}

hid_t
data_to_hdf5(int ndims, hsize_t* dims, int* data) {
    /* create a file access property and set the Core driver */
    hbool_t backing_store = 0;
    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
    herr_t status = H5Pset_fapl_core(fapl,(size_t)1,backing_store);

    /* create file */
    hid_t fileimage = H5Fcreate("nosuch.h5",H5F_ACC_EXCL,H5P_DEFAULT,fapl);
    hid_t space = H5Screate_simple(ndims,dims,NULL);
    hid_t dset = H5Dcreate(fileimage,"data",H5T_STD_I32LE,space,H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite(dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    H5Fflush(fileimage,H5F_SCOPE_GLOBAL);
    return fileimage;
}


int
main() {
    MTB_T* tb = MTB_new_tb();
    int end_of_stdin;
    hid_t fileimage_in, fileimage_out;
    hid_t dset;
    herr_t status;
    hsize_t* dims = NULL;
    int ndims = -1;
    int* data = NULL;
    size_t i;

    // Read file image in standard input.
    fileimage_in = H5UPfileimage_from_stdin(&end_of_stdin,tb);

    // Read (and close) HDF5 file.
    hdf5_to_data(fileimage_in, &dims, &ndims, &data);

    // Modif data.
    for (i = 0 ; i < reduce_mult(ndims,dims) ; i++) {
        data[i] += 99000;
    }

    // Write data in HDF5 image file.
    fileimage_out = data_to_hdf5(ndims,dims,data);

    // Write image file to standard output.
    H5UPfileimage_to_stdout(fileimage_out);
    H5Fclose(fileimage_out);

    free(data);
}
