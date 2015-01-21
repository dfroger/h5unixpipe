#include "h5unixpipe_cxx.hxx"

namespace h5unixpipe {

/* \brief C API to C++ API HDF5 file identifier
 */
H5::H5File* fileid_to_h5file(hid_t fileid)
{
    H5::H5File *h5file = new H5::H5File;
    h5file->setId(fileid);
    return h5file;
}

/* \brief Create an new emtpy HDF5 file image 
 *
 */
H5::H5File* create_new_fileimage()
{
    hbool_t backing_store = 0;
    H5::FileAccPropList fapl;
    fapl.setCore((size_t)1,backing_store);
    return new H5::H5File("nosuch.h5",H5F_ACC_TRUNC,
        H5::FileCreatPropList::DEFAULT,fapl);
}

/* \brief Write a HDF5 file image to standard output
 *
 * TODO: avoid copy
 */
void fileimage_to_stdout(H5::H5File* fileimage)
{
    // Get HDF5 image fileimagebuffer.
    fileimage->flush(H5F_SCOPE_GLOBAL);
    hid_t fileid = fileimage->getId();
    size_t fileimagebuffer_size = H5Fget_file_image(fileid, NULL, 0);
    char* fileimagebuffer = new char[fileimagebuffer_size];
    H5Fget_file_image(fileid, fileimagebuffer, fileimagebuffer_size);

    // Write fileimagebuffer to standard output.
    fwrite(fileimagebuffer,1,fileimagebuffer_size,stdout);
    delete fileimagebuffer;
}


}

