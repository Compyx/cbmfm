/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/d64.h
 * \brief   D64 disk image handling - header
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


#ifndef CBMFM_LIB_IMAGE_D64_H
#define CBMFM_LIB_IMAGE_D64_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cbmfm_types.h"
#include "base/dxx.h"


bool cbmfm_is_d64(const char *filename);

cbmfm_d64_t *   cbmfm_d64_alloc(void);
void            cbmfm_d64_init(cbmfm_d64_t *image);
cbmfm_d64_t *   cbmfm_d64_new(void);
void            cbmfm_d64_cleanup(cbmfm_d64_t *image);
void            cbmfm_d64_free(cbmfm_d64_t *image);

bool            cbmfm_d64_open(cbmfm_d64_t *image, const char *name);

uint8_t *       cbmfm_d64_bam_ptr(cbmfm_d64_t *imge);
uint8_t *       cbmfm_d64_bam_ptr_trk(cbmfm_d64_t *image, int track);
bool            cbmfm_d64_bam_get_bament(cbmfm_d64_t *image,
                                         int track,
                                         uint8_t *dest);

bool            cbmfm_d64_bam_init_bament(cbmfm_d64_t *image, int track);
void            cbmfm_d64_bam_dump(cbmfm_d64_t *image);

int             cbmfm_d64_bam_track_get_blocks_free(cbmfm_d64_t *image,
                                                    int track);

void            cbmfm_d64_get_disk_name_pet(cbmfm_d64_t *image, uint8_t *name);
void            cbmfm_d64_get_disk_name_asc(cbmfm_d64_t *image, char *name);
void            cbmfm_d64_get_disk_id_pet(cbmfm_d64_t *image, uint8_t *id);
void            cbmfm_d64_get_disk_id_asc(cbmfm_d64_t *image, char *id);

void            cbmfm_d64_set_disk_name_pet(cbmfm_d64_t *image,
                                            const uint8_t *name);
void            cbmfm_d64_set_disk_name_asc(cbmfm_d64_t *image,
                                            const char *name);

void            cbmfm_d64_set_disk_id_pet(cbmfm_d64_t *image,
                                          const uint8_t *id);
void            cbmfm_d64_set_disk_id_pet_ext(cbmfm_d64_t *image,
                                              const uint8_t *id);

void            cbmfm_d64_set_disk_id_asc(cbmfm_d64_t *image, const char *id);
void            cbmfm_d64_set_disk_id_asc_ext(cbmfm_d64_t *image,
                                              const char *id);

bool            cbmfm_d64_bam_sector_get_free(cbmfm_d64_t *image,
                                              int track, int sector,
                                              bool *state);
bool            cbmfm_d64_bam_sector_set_free(cbmfm_d64_t *image,
                                              int track,
                                              int sector,
                                              bool state);

int             cbmfm_d64_blocks_free(cbmfm_d64_t *image);


void            cbmfm_d64_dirent_parse(cbmfm_dirent_t *dirent,
                                       const uint8_t *data);

cbmfm_dir_t *   cbmfm_d64_dir_read(cbmfm_d64_t *image);


bool cbmfm_d64_file_read_from_block(cbmfm_d64_t *image,
                                    cbmfm_file_t *file,
                                    int track, int sector);
bool cbmfm_d64_file_read_from_dirent(cbmfm_dirent_t *dirent,
                                     cbmfm_file_t *file);
bool cbmfm_d64_file_read_from_index(cbmfm_d64_t *image,
                                    cbmfm_file_t *file,
                                    uint16_t index);


bool cbmfm_d64_block_write_iter_init(cbmfm_dxx_block_iter_t *iter,
                                     cbmfm_d64_t *image);
void cbmfm_d64_block_write_iter_write_data(cbmfm_dxx_block_iter_t *iter,
                                           const uint8_t *data,
                                           size_t size);
void cbmfm_d64_bam_init(cbmfm_d64_t *image);

#endif
