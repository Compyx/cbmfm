/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/t64.c
 * \brief   T64 handling
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 */

/*
 *  CbmFM - a file manager for CBM 8-bit emulation files
 *  Copyright (C) 2018  Bas Wassink <b.wassink@ziggo.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cbmfm_types.h"
#include "base.h"
#include "base/image.h"
#include "base/dir.h"
#include "log.h"

#include "t64.h"

/** \ingroup    lib_archive_t64
 * @{
 */


static void cbmfm_t64_parse_header(cbmfm_t64_t *image)
{
    uint8_t *data = image->data;
    int i;

    /* tape 'magic' */
    memcpy(image->magic, data + CBMFM_T64_HDR_MAGIC, CBMFM_T64_HDR_MAGIC_LEN);
    /* terminate */
    image->magic[CBMFM_T64_HDR_MAGIC_LEN - 1] = '\0';

    /* tape name */
    memcpy(image->tape_name, data + CBMFM_T64_HDR_TAPE_NAME,
            CBMFM_T64_HDR_TAPE_NAME_LEN);
    /* remove trailing spaces */
    i = CBMFM_T64_HDR_TAPE_NAME_LEN - 1;
    while (i >= 0 && image->tape_name[i] == 0x20) {
        image->tape_name[i] = '\0';
        i--;
    }

    /* version */
    image->version = cbmfm_word_get_le(data + CBMFM_T64_HDR_VERSION);

    /* max entries in dir */
    image->entry_max = cbmfm_word_get_le(data + CBMFM_T64_HDR_DIR_MAX);
    /* used entries in dir */
    image->entry_used = cbmfm_word_get_le(data + CBMFM_T64_HDR_DIR_USED);

    if (image->entry_used == 0) {
        cbmfm_log_warning("%s(): found entry count 0, adjusting to 1\n",
                __func__);
        cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
        image->entry_used++;
    }

}

/** \brief  Allocate a t64 image
 *
 * \return  new t64 image
 */
cbmfm_t64_t *cbmfm_t64_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_t64_t));
}


/** \brief  Initialize t64 image
 *
 * \param[in,out]   image   t64 image
 */
void cbmfm_t64_init(cbmfm_t64_t *image)
{
    cbmfm_image_init((cbmfm_image_t *)image);
    image->type = CBMFM_IMAGE_TYPE_T64;

    memset(image->magic, 0x00, CBMFM_T64_HDR_MAGIC_LEN);
    memset(image->tape_name, 0x20, CBMFM_T64_HDR_TAPE_NAME_LEN);
    image->tape_name[CBMFM_T64_HDR_TAPE_NAME_LEN] = '\0';
    image->version = 0;
    image->entry_max = 0;
    image->entry_used = 0;

    /* set read-only */
    cbmfm_image_set_readonly((cbmfm_image_t *)image, true);
}


/** \brief  Allocate an initialize a t64 image
 *
 * \return  new t64 image
 */
cbmfm_t64_t *cbmfm_t64_new(void)
{
    cbmfm_t64_t *image = cbmfm_t64_alloc();
    cbmfm_t64_init(image);
    return image;
}


/** \brief  Free members of \a image but not \a image itself
 *
 * \param[in,out]   image   t64 image
 */
void cbmfm_t64_cleanup(cbmfm_t64_t *image)
{
    cbmfm_image_cleanup((cbmfm_image_t *)image);
}


/** \brief  Free members of \a image and \a image itself
 *
 * \param[in,out]   image   t64 image
 */
void cbmfm_t64_free(cbmfm_t64_t *image)
{
    cbmfm_t64_cleanup(image);
    cbmfm_free(image);
}


/** \brief  Read data from \a path into \a image
 *
 * \param[in,out]   image   t64 image
 * \param[in]       path    path to image data
 *
 * \return  bool
 */
bool cbmfm_t64_open(cbmfm_t64_t *image, const char *path)
{
    if (!cbmfm_image_read_data((cbmfm_image_t *)image, path)) {
        return false;
    }
    cbmfm_t64_parse_header(image);
    return true;
}


/** \brief  Dump T64 header data on stdout
 *
 * \param[in]   image   t64 image
 */
void cbmfm_t64_dump_header(const cbmfm_t64_t *image)
{
    printf("tape magic    : '%s'\n", image->magic);
    printf("tape name     : '%s'\n", image->tape_name);
    printf("tape version  : $%04x\n", image->version);
    printf("dir entry max : %d\n", (int)(image->entry_max));
    printf("dir entry used: %d\n", (int)(image->entry_used));
}


#if 0
/** \brief  Determine offset in \a image of the file data segment
 *
 * \param[in]   image   t64 image
 *
 * \return  offset to file data segment
 */
static size_t t64_data_offset(cbmfm_t64_t *image)
{
    return (size_t)(CBMFM_T64_HDR_SIZE +
            image->entry_max * CBMFM_T64_DIRENT_SIZE);
}


/** \brief  Get pointer to file data segment in \a image
 *
 * \param[in]   image   t64 image
 *
 * \return  pointer to file data segment
 */
static uint8_t *t64_data_ptr(cbmfm_t64_t *image)
{
    return image->data + t64_data_offset(image);
}
#endif


/** \brief  Initialize \a dirent, including t64 specific data
 *
 * \param[out]  dirent
 */
void cbmfm_t64_dirent_init(cbmfm_dirent_t *dirent)
{
    cbmfm_dirent_t64_t *extra = &(dirent->extra.t64);

    cbmfm_dirent_init(dirent);
    extra->data_offset = 0;
    extra->load_addr = 0;
    extra->end_addr = 0;
    extra->c64s_type = 0;
    dirent->image_type = CBMFM_IMAGE_TYPE_T64;
}


/** \brief  Parse directory entry at \a index
 *
 * \param[in]   image   t64 image
 * \param[out]  dirent  directory entry
 * \param[in]   index   index in directory
 *
 * \return  false on invalid index
 */
bool cbmfm_t64_dirent_parse(cbmfm_t64_t *image,
                            cbmfm_dirent_t *dirent,
                            uint16_t index)
{
    uint8_t *data;
    cbmfm_dirent_t64_t *extra = &(dirent->extra.t64);

    if (index >= image->entry_used) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return false;
    }

    cbmfm_t64_dirent_init(dirent);
    data = image->data + CBMFM_T64_DIR_OFFSET + index * CBMFM_T64_DIRENT_SIZE;

    memcpy(dirent->filename, data + CBMFM_T64_DIRENT_FILE_NAME,
            CBMFM_CBMDOS_FILE_NAME_LEN);

    /* file type */
    extra->c64s_type = data[CBMFM_T64_DIRENT_C64S_TYPE];
    dirent->filetype = data[CBMFM_T64_DIRENT_FILE_TYPE];

    dirent->image = (cbmfm_image_t *)image;

    extra->load_addr = cbmfm_word_get_le(data + CBMFM_T64_DIRENT_LOAD_ADDR);
    extra->end_addr = cbmfm_word_get_le(data + CBMFM_T64_DIRENT_END_ADDR);
    extra->data_offset = cbmfm_dword_get_le(data + CBMFM_T64_DIRENT_DATA_OFFSET);

    /* this gets fixed later */
    dirent->filesize = (size_t)(extra->end_addr - extra->load_addr + 2);
    dirent->size_blocks = cbmfm_size_to_blocks(dirent->filesize);

    dirent->index = index;
    return true;
}


/** \brief  Function for qsort(), sorting on data offset
 *
 * \param[in]   p1  pointer to first element
 * \param[in]   p2  pointer to secend element
 *
 * \return  -1 when offset(p1) < offset(p2), 0 when offset(p1), == offset(p2),
 *          +1 when offset(p1) > offset(p2)
 */
static int compar_offset(const void *p1, const void *p2)
{
    const cbmfm_dirent_t *d1 = *(const cbmfm_dirent_t * const *)p1;
    const cbmfm_dirent_t *d2 = *(const cbmfm_dirent_t * const *)p2;

    cbmfm_log_debug("d1 offset = $%" PRIx32 ", d2 offset = $%" PRIx32 "\n",
            d1->extra.t64.data_offset, d2->extra.t64.data_offset);

    if (d1->extra.t64.data_offset < d2->extra.t64.data_offset) {
        return -1;
    } else if (d1->extra.t64.data_offset > d2->extra.t64.data_offset) {
        return 1;
    } else {
        return 0;
    }
}


/** \brief  Function for qsort(), sorting on directory index
 *
 * \param[in]   p1  pointer to first element
 * \param[in]   p2  pointer to secend element
 *
 * \return  -1 when index(p1) < index(p2), 0 when index(p1), == index(p2),
 *          +1 when index(p1) > index(p2)
 */
static int compar_index(const void *p1, const void *p2)
{
    const cbmfm_dirent_t *d1 = *(const cbmfm_dirent_t * const *)p1;
    const cbmfm_dirent_t *d2 = *(const cbmfm_dirent_t * const *)p2;

    cbmfm_log_debug("d1 index = $%" PRIx32 ", d2 offset = $%" PRIx32 "\n",
            d1->index, d2->index);

    if (d1->index < d2->index) {
        return -1;
    } else if (d1->index > d2->index) {
        return 1;
    } else {
        return 0;
    }
}


/** \brief  Fix possible corruption in \a dir
 *
 * \param[in,out]   dir t64 directory
 *
 * \return  number of fixes applied
 */
static int cbmfm_t64_fix_dir(cbmfm_dir_t *dir)
{
    cbmfm_t64_t *image = (cbmfm_t64_t *)(dir->image);
    size_t rep_size;    /* reported size */
    size_t act_size;    /* actual size */
    int fixes = 0;
    uint16_t index;

    if (image->entry_max == 0) {
        cbmfm_log_warning("adjusting dir max entry count from 0 to 1\n");
        fixes++;
        image->entry_max = 1;
    }

    /* sort entries based on data-offset */
    qsort(dir->entries, (size_t)(dir->entry_used),
            sizeof(cbmfm_dirent_t *), compar_offset);

    for (index = 0; index < image->entry_used; index++) {
        cbmfm_dirent_t *dirent = dir->entries[index];
        cbmfm_dirent_t *next = NULL;
        cbmfm_dirent_t64_t *t64 = &(dirent->extra.t64);

        /* skip C64s FRZ files */
        if (t64->c64s_type > 1) {
            continue;
        }

        /* get next entry to compare against */
        if (index < dir->entry_used - 1) {
            next = dir->entries[index + 1];
        } else {
            next = NULL;
        }

        /* get size according to directory entries */
        rep_size = (size_t)(t64->end_addr - t64->load_addr);

        /* determine actual size */
        if (next != NULL) {
            act_size = next->extra.t64.data_offset - t64->data_offset;
        } else {
            act_size = image->size - t64->data_offset;
        }
        cbmfm_log_debug("load = $%04x, end = $%04x, reported size = $%04x, "
                "actual size = $%04x\n",
                t64->load_addr, t64->end_addr,
                (unsigned int)rep_size,
                (unsigned int)act_size);
        if (act_size != rep_size) {
            cbmfm_log_debug("got invalid end address, fixing\n");

            if (index == dir->entry_max -1 && rep_size < act_size) {
                /* don't fix last record when actual size is larger, some
                 * T64's appear to have padding for the last record */
                continue;
            }
            t64->end_addr = (uint16_t)(t64->load_addr + act_size);
            fixes++;
        }

        /* set proper filesize and size_blocks */
        dirent->filesize = (size_t)(t64->end_addr - t64->load_addr + 2);
        dirent->size_blocks = cbmfm_size_to_blocks(dirent->filesize);

    }



    /* return directory to original state by sorting on index */
    qsort(dir->entries, (size_t)(dir->entry_used),
            sizeof(cbmfm_dirent_t *), compar_index);

    if (fixes > 0) {
        cbmfm_log_debug("fixed %d errors\n", fixes);
        cbmfm_image_set_dirty(dir->image, true);
    }

    return fixes;
}


/** \brief  Read directory of \a image
 *
 * This function reads the directory of \a image and tries to fix any corruption
 * in the directory entries.
 *
 * \param[in]   image   t64 image
 *
 * \return  directory or `NULL` on failure
 *
 * \note    The caller is responsible for calling cbmfm_dir_free() on the
 *          returned pointer to free memory used by the directory.
 */
cbmfm_dir_t *cbmfm_t64_read_dir(cbmfm_t64_t *image)
{
    cbmfm_dir_t *dir;
    uint16_t index;

    dir = cbmfm_dir_new();
    dir->image = (cbmfm_image_t *)image;

    for (index = 0; index < image->entry_used; index++) {
        cbmfm_dirent_t dirent;

        cbmfm_log_debug("parsing entry %" PRIx16 "\n", index);
        if (!cbmfm_t64_dirent_parse(image, &dirent, index)) {
            cbmfm_dir_free(dir);
            return NULL;
        }

        cbmfm_dir_append_dirent(dir, &dirent);
    }

    /* now fix the end addres fields */
    cbmfm_t64_fix_dir(dir);
    return dir;
}





/** \brief  Extract file at \a index in \a dir
 *
 * Save a file in \a dir at \a index to the host file system. When \a name is
 * `NULL` the PETSCII file name will be translated to ASCII in a encoding that
 * the host OS can handle (meaning on Windows you'll see a lot more
 * substitutions of PETSCII chars than on Unix), and a '.prg' suffix added.
 *
 * This function requires first getting a directory of a T64 image, since those
 * T64 images tend to have a lot of corruptions and reading the dir of a T64
 * image fixes (or at least tries) those corruptions.
 *
 * \param[in]   dir     t64 directory
 * \param[in]   index   index of file in directory
 * \param[in]   name    filename (use `NULL` to use the PETSCII filename)
 *
 * \return  bool
 */
bool cbmfm_t64_extract_file(cbmfm_dir_t *dir,
                            uint16_t index,
                            const char *name)
{
    cbmfm_t64_t *image = (cbmfm_t64_t *)(dir->image);
    cbmfm_dirent_t *dirent;
    uint8_t *ptr;
    uint8_t *data;
    bool result;

    /* +5 = '.ext' + '\0' */
    char filename[CBMFM_CBMDOS_FILE_NAME_LEN + 5];

    if (index >= dir->entry_used) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return false;
    }

    /* get dirent, data pointer */
    dirent = dir->entries[index];
    ptr = image->data + dirent->extra.t64.data_offset;
    /* allocate memory for file data */
    data = cbmfm_malloc(dirent->filesize);

    /* store load address */
    cbmfm_word_set_le(data, dirent->extra.t64.load_addr);
    /* copy file data */
    memcpy(data + 2, ptr, dirent->filesize - 2);

    if (name == NULL) {
        cbmfm_pet_filename_to_host(filename, dirent->filename, "prg");
    }

    result = cbmfm_write_file(data, dirent->filesize,
            name != NULL ? name : filename);

    cbmfm_free(data);
    return result;
}


/** \brief  Extract all files from \a dir
 *
 * Extracts all files in the directory, using their PETSCII file names converted
 * to the host encoding and suffixed with '.prg'.
 *
 * \param[in]   dir t64 directory
 *
 * \return  bool
 */
bool cbmfm_t64_extract_all(cbmfm_dir_t *dir)
{
    uint16_t index;

    for (index = 0; index < dir->entry_used; index++) {
        if (!cbmfm_t64_extract_file(dir, index, NULL)) {
            return false;
        }
    }
    return true;
}


/** @} */
