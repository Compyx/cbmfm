/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test_lib_base_zipcode.c
 * \brief   Unit test for src/lib/base/zipcode.c
 *
 * Tests generic zipcode functions.
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
#include <inttypes.h>
#include <string.h>


#include "lib/base/errors.h"
#include "lib/base/mem.h"

#include "testcase.h"

#include "lib/base/zipcode.h"


/** \brief  Test case for the zipcode unpacking function
 */
typedef struct zc_unpack_test_s {
    const char *desc;       /**< short description of test */
    const int * data;       /**< test data, an array of int's, -1 terminated */
    int         result;     /**< expected result */
    int         errcode;    /**< expected error code if result == -1 */
} zc_unpack_test_t;


/** \brief  unpack test: store 256 bytes in block
 *
 * This data is supposed to result in 256 bytes being 'stored' in (18,0) as-is.
 */
static const int unp_test_store[] = {
    18 | 0x00,  /* track 18, method: store */
    0,          /* sector 0 */

    /* data: all values from 0-255: */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    -1
};


/** \brief  unpack test: fill sector with a single value
 *
 * This data is supposed to result in 256 bytes being 'stored' in (18,0) as-is.
 */
static const int unp_test_fill[] = {
    18 | 0x40,  /* track 18, method: fill */
    0,          /* sector 0 */
    0xff,
    -1
};


/** \brief  unpack test: RLE decode data into a 256-byte block
 *
 * This data is supposed to result in 256 bytes.
 */
static const int unp_test_rle_valid[] = {
    18 | 0x80,  /* track 18, method: RLE */
    0,          /* sector 0 */
    25,          /* RLE data size */
    0xff,          /* RLE pack byte */

    /* RLE data: size = 8 + 3 + 8 + 3 + 3 = 25 */
    0, 1, 2, 3, 4, 5, 6, 7,    /* $00-$07; 0-7 */
    0xff, 8, 8,             /* $08-$0f: 8 * 8 */
    16, 17, 18, 19, 20, 21, 22, 23, /* $10-$17: 16-23 */
    0xff, 8, 24,            /* $18-$1f: 8 * 24 */
    0xff, 0xe0, 0xaa,       /* $20-$ff: 240 * $aa */
    -1
};


/** \brief  unpack test: RLE decode data into a 256-byte block
 *
 * This data is supposed to cause a buffer underflow
 */
static const int unp_test_rle_underflow[] = {
    18 | 0x80,
    0,
    7,
    0xff,

    /* RLE data, but only fills 32 bytes */
    0x00, 0x01, 0x02, 0x03,
    0xff, 32-4, 0xaa,

    -1
};


/** \brief  unpack test: RLE decode data into a 256-byte block
 *
 * This data is supposed to cause a buffer overflow
 */
static const int unp_test_rle_overflow[] = {
    18 | 0x80,
    0,
    7,
    0xff,

    /* RLE data, but only fills 32 bytes */
    0x00, 0x01, 0x02, 0x03,
    0xff, 0xff, 0xaa,

    -1
};
/** \brief  List of tests for the unpack function
 */
static const zc_unpack_test_t unpack_tests[] = {
    { "unpack stored sector (18,0)",
        unp_test_store,
        258, CBMFM_ERR_OK },
    { "fill sector (18,0) with a single value ($ff)",
        unp_test_fill,
        3, CBMFM_ERR_OK },
    { "RLE decode sector (18,0) (valid data)",
        unp_test_rle_valid,
        29, CBMFM_ERR_OK },
    { "RLE decode sector with underflow",
        unp_test_rle_underflow,
        -1, CBMFM_ERR_BUFFER_UNDERFLOW },
    { "RLE decode sector with overflow",
        unp_test_rle_overflow,
        -1, CBMFM_ERR_BUFFER_OVERFLOW },

    { NULL, NULL, 0, 0 }
};




static bool setup(void);
static void teardown(void);
static bool test_lib_base_zipcode_unpack(struct test_case_s *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_base_zipcode[] = {
    { "unpack", "test unpacking of a zipcoded block",
        test_lib_base_zipcode_unpack, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base library functions
 */
test_module_t module_lib_base_zipcode = {
    "zipcode",
    "base zipcode handling",
    tests_lib_base_zipcode,
    setup,
    teardown,
    0, 0
};






/** \brief  Setup function
 *
 * This runs before starting the tests, allowing to set up some resources.
 *
 * \return  bool
 */
static bool setup(void)
{
    return true;
}


/** \brief  Teardown function
 *
 * This runs after running the tests, allowing to clean up resource set up
 * by setup().
 */
void teardown(void)
{
    return;
}



/** \brief  Dummy test
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_base_zipcode_unpack(struct test_case_s *test)
{
    uint8_t buffer[256];
    uint8_t testdata[258];
    int t;

    printf("..... Running %s()\n", __func__);
    for (t = 0; unpack_tests[t].desc != NULL; t++) {
        int i;
        int result;
        bool success;

        test->total++;
        printf("...... Running sub test '%s':\n", unpack_tests[t].desc);
        memset(buffer, 0x00, 256);
        memset(testdata, 0x00, 258);

        cbmfm_errno = CBMFM_ERR_OK;

        /* copy test data */
        for (i = 0; unpack_tests[t].data[i] >= 0; i++) {
            testdata[i] = (uint8_t)unpack_tests[t].data[i];
        }
        printf("........ Copying %d bytes of test data\n", i);
        printf("........ Running cbmfm_zipcode_unpack():\n");
        result = cbmfm_zipcode_unpack(buffer, testdata);
        success = result == unpack_tests[t].result;
        printf("........ Result = %d, expected %d -> %s\n",
                result, unpack_tests[t].result,
                success ? "OK" : "failed");
        printf("........ Checking error code: %d (%s):\n",
                cbmfm_errno, cbmfm_strerror(cbmfm_errno));
        if (cbmfm_errno != unpack_tests[t].errcode) {
            printf("........ Failed, expected %d (%s)\n",
                    unpack_tests[t].errcode,
                    cbmfm_strerror(unpack_tests[t].errcode));
            test->failed++;
        } else {
            printf("........ OK\n");
        }
        if (!success) {
            cbmfm_perror("....... Error:");
            test->failed++;
        }
        cbmfm_hexdump(buffer, 0, 256);

    }



    return true;
}
