#ifndef H5UNIXPIPE_HXX_INCLUDED
#define H5UNIXPIPE_HXX_INCLUDED

#include <stdexcept>
#include <stdlib.h>
#include <iostream>

#include "hdf5.h"
#include "H5Cpp.h"
#include "H5LTpublic.h"

namespace h5unixpipe {

H5::H5File* fileid_to_h5file(hid_t fileid);

H5::H5File* create_new_fileimage();

void fileimage_to_stdout(H5::H5File* fileimage);


} // namespace

#endif
