#ifndef H5UNIXPIPE_H_INCLUDED
#define H5UNIXPIPE_H_INCLUDED

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "minimal_traceback.h"

hid_t
H5UPfileimage_from_stdin(int* end_of_stdin, MTB_T* tb);

void
H5UPfileimage_to_stdout(hid_t fileimage);


#endif
