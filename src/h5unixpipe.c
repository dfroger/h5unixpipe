#include "h5unixpipe.h"

static const char* FORMAT_SIGNATURE = "\211HDF\r\n\032\n";
static size_t SSGNT = 8;

static int
is_signt(char* eight_bytes);

static void
extend_buffer(char** buffer, size_t old_size, size_t new_size, MTB_T* tb);

static void
read_until_format_signature(char** buffer, char first_byte, size_t* isgnt, MTB_T* tb);

static void
parse_super_block(char** tmp, size_t* base_address, size_t* eof_address, MTB_T* tb);

static void
read_all_image(char** buffer, char* tmp, size_t isgnt, size_t file_size, MTB_T* tb);

static void
read_buffer_from_stdin(char** buffer, size_t* file_size, int *end_of_stdin, MTB_T* tb);

static hid_t
buffer_to_fileimage_id(char* buffer, size_t buffer_size);


/* Returns 1 if eight_bytes points to HDF5 format signature */
int
is_signt(char* eight_bytes) {
    return strncmp(eight_bytes,FORMAT_SIGNATURE,SSGNT) == 0 ? 1 : 0;
}

void
extend_buffer(char** buffer, size_t old_size, size_t new_size, MTB_T* tb) {
    /* Make buffer bigger. */
    *buffer = realloc(*buffer, new_size);
    if (*buffer == NULL) {
        MTB_raise_MemoryError_V("Failed to realloc array");
    }

    /* Read new bytes in stdin */
    int to_read = new_size - old_size;
    int readen = fread(*buffer+old_size, 1, to_read, stdin);
    if (readen != to_read) {
        MTB_raise_IOError_V("Failed to read in standard input");
    }
}

/* Read standard input:
 *     from the first byte of: the file
 *     to the last byte of   : the format signature
 *
 * Return a buffer allocated with size isgnt+SSGNT, containing
 * data readen in stdin.
 *
 * We read in stdin (isgnt+SSGNT) bytes, and check is the string
 * between isgnt and isgnt+SSGNT is the format signature.
 * 
 * We look at isgnt = 0, 512, 1024, 2048 etc, until we find it.
 */
void
read_until_format_signature(char** buffer, char first_byte, size_t* isgnt, MTB_T* tb) {
    /* Read first 8 bytes */
    *isgnt = 0;
    *buffer = malloc(1);
    memcpy(*buffer,&first_byte,1);
    extend_buffer(buffer,1,SSGNT,tb); MTB_V

    /* Look for format signature. */
    if ( is_signt(*buffer) )
        return;

    size_t isgnt_new = 512;
    while (1) {
        /* Read more bytes in stdin. */
        extend_buffer(buffer, *isgnt+SSGNT, isgnt_new+SSGNT, tb); MTB_V
        *isgnt = isgnt_new;
        isgnt_new *= 2;

        /* Look for format signature. */
        if ( is_signt(*buffer + *isgnt) )
            break;
    }
}

/* Read in stdin:
 *    from first byte of: superblock version
 *    to last bytye of  : end of file address
 */
void
parse_super_block(char** tmp, size_t* base_address, size_t* eof_address, MTB_T* tb) {

    /* Check superblock version */
    *tmp = NULL;
    extend_buffer(tmp, 0, 1, tb); MTB_V
    size_t superblock_version = (size_t) **tmp;
    if (superblock_version != 0) {
        MTB_raise_NotImplementedError_V("Only superblock_version 0 is supported for now.")
    }

    /* check size of offsets and lengts */
    extend_buffer(tmp, 1, 16, tb); MTB_V
    size_t size_of_offsets = (size_t) (*tmp)[5];
    if (size_of_offsets != 8) {
        MTB_raise_NotImplementedError_V("Only size_of_offsets 8 is supported for now.")
    }
    size_t size_of_lengths = (size_t) (*tmp)[6];
    if (size_of_lengths != 8) {
        MTB_raise_NotImplementedError_V("Only size_of_lengths 8 is supported for now.")
    }

    /* read base and end of file address */
    extend_buffer(tmp, 16, 40, tb); MTB_V
    memcpy(base_address, (size_t*) (*tmp+16), 8);
    memcpy(eof_address, (size_t*) (*tmp+32), 8);
}

/* Read in stdin:
 *    from first byte of: driver information block address
 *    to last bytye of  : end of file
 */
void
read_all_image(char** buffer, char* tmp, size_t isgnt, 
               size_t file_size, MTB_T* tb) {

    *buffer = realloc(*buffer, file_size);
    if (*buffer == NULL) {
        MTB_raise_MemoryError_V("Failed to realloc buffer");
    }

    memcpy(*buffer+isgnt+SSGNT, tmp, 40);

    size_t to_read = file_size - (isgnt+SSGNT+40);

    int readen = fread(*buffer+isgnt+SSGNT+40, 1, to_read, stdin);
    if (readen != to_read) {
        MTB_raise_IOError_V("Failed to read in standard input");
    }
}

void
read_buffer_from_stdin(char** buffer, size_t* file_size, int *end_of_stdin, MTB_T* tb) {
    int readen;
    char first_byte;
    readen = fread(&first_byte, 1, 1, stdin);
    if (readen!=1) {
        *end_of_stdin = 1;
        return;
    } else {
        *end_of_stdin = 0;
    }

    size_t isgnt; 
    read_until_format_signature(buffer,first_byte,&isgnt,tb); MTB_V

    char* tmp;
    size_t base_address, eof_address;
    parse_super_block(&tmp,&base_address,&eof_address,tb); MTB_V

    *file_size = eof_address - base_address;
    read_all_image(buffer,tmp,isgnt,*file_size,tb);
    free(tmp);
}

/* \brief Open an HDF5 file image from its buffer.
 */
hid_t buffer_to_fileimage_id(char* buffer, size_t buffer_size) {

    //unsigned flags = H5LT_FILE_IMAGE_DONT_COPY | H5LT_FILE_IMAGE_DONT_RELEASE;
    unsigned flags = H5LT_FILE_IMAGE_DONT_COPY;
    hid_t fileimage = H5LTopen_file_image(buffer, buffer_size, flags);
    return fileimage;
}

/*
 *
 */
hid_t
H5UPfileimage_from_stdin(int* end_of_stdin, MTB_T* tb) {
    char* buffer=NULL;
    size_t file_size;
    read_buffer_from_stdin(&buffer, &file_size, end_of_stdin,tb); MTB_S
    if (*end_of_stdin==1) 
        return -1;
    hid_t fileimage = buffer_to_fileimage_id(buffer, file_size);
    return fileimage;
}

/* \brief Write a HDF5 file image to standard output
 */
void
H5UPfileimage_to_stdout(hid_t fileimage)
{
    // TODO: flush before.  H5Fflush(file,H5F_SCOPE_GLOBAL);
    // TODO: avoid copy
    size_t fileimagebuffer_size = H5Fget_file_image(fileimage, NULL, 0);
    char* fileimagebuffer = malloc(fileimagebuffer_size);
    H5Fget_file_image(fileimage, fileimagebuffer, fileimagebuffer_size);

    // Write fileimagebuffer to standard output.
    fwrite(fileimagebuffer,1,fileimagebuffer_size,stdout);
    free(fileimagebuffer);
}
