/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/d64.c
 * \brief   D64 disk image handling
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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
#include "base/dxx.h"

#include "d64.h"


/** \brief  Speed zones for D64 images
 */
static const cbmfm_dxx_speedzone_t zones_d64[] = {
    {  1, 17, 21 },
    { 18, 24, 19 },
    { 25, 30, 18 },
    { 31, 40, 17 },
    { -1, -1, -1 }
};


/** \brief  Allocate a d64 image object
 *
 * \return  heap-allocated d64 image object, uninitialized
 */
cbmfm_d64_t *cbmfm_d64_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_d64_t));
}


/** \brief  Initialize \a image to a usable state
 *
 * \param[in,out]   image   d64 image
 */
void cbmfm_d64_init(cbmfm_d64_t *image)
{
    cbmfm_image_init((cbmfm_image_t *)image);
    image->type = CBMFM_IMAGE_TYPE_D64;
    image->zones = zones_d64;
}


/** \brief  Allocate and initialize a d64 image
 *
 * \return  new d64 image
 */
cbmfm_d64_t *cbmfm_d64_new(void)
{
    cbmfm_d64_t *image = cbmfm_d64_alloc();
    cbmfm_d64_init(image);
    return image;
}


/** \brief  Free members of \a image, but not \a image itself
 *
 * \param[in,out]   image   d64 image
 */
void cbmfm_d64_cleanup(cbmfm_d64_t *image)
{
    cbmfm_image_cleanup((cbmfm_image_t *)image);
}


/** \brief  Free members of \a image and \a image itself
 *
 * \param[in,out]   image   d64 image
 */
void cbmfm_d64_free(cbmfm_d64_t *image)
{
    cbmfm_d64_cleanup(image);
    cbmfm_free(image);
}


/** \brief  Read d64 file \a name into \a image
 *
 * \param[in,out]   image   d64 image
 * \param[in]       name    image file name
 *
 * \return  true on success
 *
 * \throw   #CBMFM_ERR_IO
 * \throw   #CBMFM_ERR_SIZE_MISMATCH
 */
bool cbmfm_d64_open(cbmfm_d64_t *image, const char *name)
{
    if (!cbmfm_image_read_data((cbmfm_image_t *)image, name)) {
        return false;
    }

    /* check size, set track count & error bytes */
    switch (image->size) {
        case CBMFM_D64_SIZE_STD:
            image->track_max = 35;
            image->errors = false;
            break;
        case CBMFM_D64_SIZE_STD_ERR:
            image->track_max = 35;
            image->errors = true;
            break;
        case CBMFM_D64_SIZE_EXT:
            image->track_max = 40;
            image->errors = false;
            break;
        case CBMFM_D64_SIZE_EXT_ERR:
            image->track_max = 40;
            image->errors = true;
            break;
        default:
            /* invalid size */
            cbmfm_free(image->data);
            image->data = NULL;
            cbmfm_errno = CBMFM_ERR_SIZE_MISMATCH;
            return false;
    }
    return true;
}


/** \brief  Get pointer to BAM of \a image
 *
 * \param   image   d64 image
 *
 * \return  pointer to BAM
 */
uint8_t *cbmfm_d64_bam_ptr(cbmfm_d64_t *image)
{
    return image->data + cbmfm_dxx_block_offset(image->zones,
                                                CBMFM_D64_BAM_TRACK,
                                                CBMFM_D64_BAM_SECTOR);
}


/** \brief  Get pointer to BAM entry for \a track
 *
 * \param[in]   image   d64 image
 * \param[in]   track   track number
 *
 * \return  pointer to track entry or `NULL` on illegal track number
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 */
uint8_t *cbmfm_d64_bam_ptr_trk(cbmfm_d64_t *image, int track)
{
    if (track < 1 || track > image->track_max) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_TRACK;
        return NULL;
    }
    return cbmfm_d64_bam_ptr(image) + CBMFM_D64_BAM_ENTRIES
        + ((track - 1) * CBMFM_D64_BAMENT_SIZE);
}


/** \brief  Copy the BAM entry for \a track into \a dest
 *
 * The \a dest argument should point to at least #CBMFM_D64_BAMENT_SIZE (4)
 * bytes.
 *
 * \param[in]   image   d64 image
 * \param[in]   track   track number
 * \param[out]  dest    destination of BAM entry data
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 */
bool cbmfm_d64_bam_get_bament(cbmfm_d64_t *image, int track, uint8_t *dest)
{
    uint8_t *ptr = cbmfm_d64_bam_ptr_trk(image, track);

    if (ptr == NULL) {
        return false;
    }

    memcpy(dest, ptr, CBMFM_D64_BAMENT_SIZE);
    return true;
}



/** \brief  Get disk name in PETSCII of \a image
 *
 * Get the disk name of \a image and store it in \a name. The \a name argument
 * most point to valid memory of at least 16 bytes.
 *
 * \param[in]   image   d64 image
 * \param[out]  name    destination of disk name
 */
void cbmfm_d64_get_disk_name_pet(cbmfm_d64_t *image, uint8_t *name)
{
    uint8_t *bam = cbmfm_d64_bam_ptr(image);

    memcpy(name, bam + CBMFM_D64_BAM_DISK_NAME, CBMFM_CBMDOS_DISK_NAME_LEN);
}


/** \brief  Get disk name in ASCII of \a image
 *
 * Get the disk name of \a image, translate to ASCII and store it in \a name.
 * The \a name argument most point to valid memory of at least 17 bytes (disk
 * name of 16 bytes + 1 byte for nil).
 *
 * \param[in]   image   d64 image
 * \param[out]  name    destination of disk name
 */
void cbmfm_d64_get_disk_name_asc(cbmfm_d64_t *image, char *name)
{
    uint8_t pet[CBMFM_CBMDOS_DISK_NAME_LEN];

    cbmfm_d64_get_disk_name_pet(image, pet);
    cbmfm_pet_to_asc_str(name, pet, CBMFM_CBMDOS_DISK_NAME_LEN);
}


/** \brief  Get PETSCII disk ID
 *
 * The disk ID returned is the 'extended' ID and requires 5 bytes of storage.
 *
 * \see #CBMFM_CBMDOS_DISK_ID_LEN_EXT
 *
 * \param[in]   image   d64 image
 * \param[out]  id      disk ID (5 bytes)
 */
void cbmfm_d64_get_disk_id_pet(cbmfm_d64_t *image, uint8_t *id)
{
    uint8_t *bam = cbmfm_d64_bam_ptr(image);

    memcpy(id, bam + CBMFM_D64_BAM_DISK_ID, CBMFM_CBMDOS_DISK_ID_LEN_EXT);
}


/** \brief  Get ASCII disk ID
 *
 * The disk ID returned is the 'extended' ID and requires 6 bytes of storage.
 * (5 bytes for the extended ID, 1 byte for nil)
 *
 * \see #CBMFM_CBMDOS_DISK_ID_LEN_EXT
 *
 * \param[in]   image   d64 image
 * \param[out]  id      disk ID (5 bytes)
 */
void cbmfm_d64_get_disk_id_asc(cbmfm_d64_t *image, char *id)
{
    uint8_t pet[CBMFM_CBMDOS_DISK_ID_LEN_EXT];

    cbmfm_d64_get_disk_id_pet(image, pet);
    cbmfm_pet_to_asc_str(id, pet, CBMFM_CBMDOS_DISK_ID_LEN_EXT);
}


/** \brief  Set disk name using PETSCII
 *
 * \param[in,out]   image   d64 image
 * \param[in]       name    PETSCII disk name (16 bytes)
 */
void cbmfm_d64_set_disk_name_pet(cbmfm_d64_t *image, const uint8_t *name)
{
    uint8_t *bam = cbmfm_d64_bam_ptr(image);

    memcpy(bam + CBMFM_D64_BAM_DISK_NAME, name, CBMFM_CBMDOS_DISK_NAME_LEN);
    cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
}


/** \brief  Set disk name using ASCII
 *
 * Sets disk name using at most 16 bytes of ASCII string \a name, the remaining
 * bytes (if any) are padded with 0.
 *
 * \param[in,out]   image   d64 image
 * \param[in]       name    ASSCII disk name
 */
void cbmfm_d64_set_disk_name_asc(cbmfm_d64_t *image, const char *name)
{
    uint8_t pet[CBMFM_CBMDOS_DISK_NAME_LEN];

    cbmfm_asc_to_pet_str(pet, name, CBMFM_CBMDOS_DISK_NAME_LEN);
    cbmfm_d64_set_disk_name_pet(image, pet);
    cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
}


/** \brief  Set disk ID, using \a len bytes of \a id
 *
 * \param[in,out]   image   d64 image
 * \param[in]       id      disk ID in PETSCII
 * \param[in]       len     length of \a id
 */
static void set_disk_id_pet(cbmfm_d64_t *image, const uint8_t *id, size_t len)
{
    uint8_t *bam = cbmfm_d64_bam_ptr(image);

    memcpy(bam + CBMFM_D64_BAM_DISK_ID, id, len);
    cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
}


/** \brief  Set standard 2-byte disk ID
 *
 * \param[in,out]   image   d64 image
 * \param[in]       id      2-byte disk ID
 */
void cbmfm_d64_set_disk_id_pet(cbmfm_d64_t *image, const uint8_t *id)
{
    set_disk_id_pet(image, id, CBMFM_CBMDOS_DISK_ID_LEN);
    cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
}


/** \brief  Set extended 5-byte disk ID
 *
 * \param[in,out]   image   d64 image
 * \param[in]       id      5-byte disk ID
 */
void cbmfm_d64_set_disk_id_pet_ext(cbmfm_d64_t *image, const uint8_t *id)
{
    set_disk_id_pet(image, id, CBMFM_CBMDOS_DISK_ID_LEN_EXT);
    cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
}


/** \brief  Set disk ID, using at most \a len bytes of \a id
 *
 * \param[in,out]   image   d64 image
 * \param[in]       id      disk ID in ASCII
 * \param[in]       len     max length of \a id
 */
static void set_disk_id_asc(cbmfm_d64_t *image, const char *id, size_t len)
{
    uint8_t *bam;
    uint8_t pet[CBMFM_CBMDOS_DISK_ID_LEN_EXT];

    bam = cbmfm_d64_bam_ptr(image);
    cbmfm_asc_to_pet_str(pet, id, len);
    memcpy(bam + CBMFM_D64_BAM_DISK_ID, pet, len);
    cbmfm_image_set_dirty((cbmfm_image_t *)image, true);
}


/** \brief  Set standard 2-byte disk ID
 *
 * Copies at most 2 bytes of \a id.
 *
 * \param[in,out]   image   d64 image
 * \param[in]       id      2-byte disk ID
 */
void cbmfm_d64_set_disk_id_asc(cbmfm_d64_t *image, const char *id)
{
    set_disk_id_asc(image, id, CBMFM_CBMDOS_DISK_ID_LEN);
}


/** \brief  Set extended 5-byte disk ID
 *
 * Copies at most 5 bytes of \a id.
 *
 * \param[in,out]   image   d64 image
 * \param[in]       id      5-byte disk ID
 */
void cbmfm_d64_set_disk_id_asc_ext(cbmfm_d64_t *image, const char *id)
{
    set_disk_id_asc(image, id, CBMFM_CBMDOS_DISK_ID_LEN_EXT);
}


/** \brief  Determine if (\a track,\a sector) is free or used
 *
 * The boolean value stored in \a state indicates whether the block (\a track,
 * \a sector) is free (true == free, false == used).
 *
 * \param[in]   image   d64 image
 * \param[in]   track   track number
 * \param[in]   sector  sector number
 * \param[out]  state   free state target
 *
 * \return  true if the input was valid
 */
bool cbmfm_d64_bam_sector_get_free(
        cbmfm_d64_t *image,
        int track,
        int sector,
        bool *state)
{
    int blk_count;
    uint8_t bament[CBMFM_D64_BAMENT_SIZE];
    int offset;
    unsigned int mask;

    blk_count = cbmfm_dxx_track_block_count((cbmfm_dxx_image_t *)image, track);
    if (blk_count < 0) {
        return false;
    }
    if (sector >= blk_count) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_SECTOR;
        return false;
    }

    cbmfm_d64_bam_get_bament(image, track, bament);

    /* offset in bament of bitmap byte */
    offset = 1 + sector / 8;
    mask = 1U << (sector % 8);

    *state = bament[offset] & mask ? true : false;

    return true;
}


/** \brief  Get number of free blocks for \a track in \a image
 *
 * \param[in]   image   d64 image
 * \param[in]   track   track number
 *
 * \return  number of free blocks or -1 on error
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 */
int cbmfm_d64_bam_track_get_blocks_free(cbmfm_d64_t *image, int track)
{
    uint8_t bament[CBMFM_D64_BAMENT_SIZE];
    int blk_count;
    int blocks;
    uint8_t mask;

    blk_count = cbmfm_dxx_track_block_count((cbmfm_dxx_image_t *)image, track);
    if (blk_count < 0) {
        return -1;
    }

    /* get bament */
    cbmfm_d64_bam_get_bament(image, track, bament);

    /* every track in a Dxx image has at least 17 blocks per track: */
    blocks = cbmfm_popcount_byte(bament[1]) + cbmfm_popcount_byte(bament[2]);

    /* create bitmask to filter out the bits that matter for track 17+ */
    mask = (uint8_t)((1U << (blk_count - 16)) - 1U);

    return blocks + cbmfm_popcount_byte(bament[3] & mask);
}


/** \brief  Dump BAM track entries on stdout
 *
 * \param[in]   image   d64 image
 */
void cbmfm_d64_bam_dump(cbmfm_d64_t *image)
{
    int track;

    printf("                  11111111112\n");
    printf("TRK SF  012345678901234567890\n");

    for (track = 1; track <= image->track_max; track++) {
        uint8_t bament[CBMFM_D64_BAMENT_SIZE];
        int sector;
        int sec_count = cbmfm_dxx_track_block_count((cbmfm_dxx_image_t *)image,
                track);

        memcpy(bament, cbmfm_d64_bam_ptr_trk(image, track), CBMFM_D64_BAMENT_SIZE);
        printf("%2d: %2d  ", track, bament[0]);
        for (sector = 0; sector < sec_count; sector++) {
            bool state = false;

            cbmfm_d64_bam_sector_get_free(image, track, sector, &state);
            putchar(state ?  '*' : '-');
        }
        putchar('\n');

    }
}


/** \brief  Calculate number of blocks free in \a image
 *
 * Calculates blocks free by examining the BAM bitmaps for each track.
 *
 * \param[in]   image   d64 image
 *
 * \return  blocks free
 */
int cbmfm_d64_blocks_free(cbmfm_d64_t *image)
{
    int track;
    int blocks = 0;

    for (track = 1; track <= image->track_max; track++) {
        /* skip BAM/directory track */
        if (track != CBMFM_D64_DIR_TRACK) {
            blocks += cbmfm_d64_bam_track_get_blocks_free(image, track);
        }
    }
    return blocks;
}
