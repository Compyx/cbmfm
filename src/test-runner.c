/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/test-runner.c
 * \brief   Test driver for cbmfm
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

/** \defgroup   tests   Unit testing
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "lib/base/errors.h"
#include "lib/base/log.h"

#include "testcase.h"

/*
 * Test modules
 */
#include "test_lib_base.h"
#include "test_lib_image_ark.h"
#include "test_lib_image_d64.h"
#include "test_lib_base_dxx.h"
#include "test_lib_base_dir.h"
#include "test_lib_image_t64.h"
#include "test_lib_image_lnx.h"
#include "test_lib_base_zipcode.h"


/** \brief  Print usage/help message on stdout
 */
static void usage(void)
{
    printf("Usage: test-runner <module> [<tests>]\n");
    printf("\n");
    printf("Options:\n");
    printf("  --help                    display help\n");
    printf("  --list-modules            list available modules\n");
    printf("  --list-tests <module>     list tests in <module>\n");
}


/** \brief  Register test modules
 */
static void register_modules(void)
{
    test_module_register(&module_lib_base);
    test_module_register(&module_lib_image_ark);
    test_module_register(&module_lib_base_dxx);
    test_module_register(&module_lib_image_d64);
    test_module_register(&module_lib_base_dir);
    test_module_register(&module_lib_image_t64);
    test_module_register(&module_lib_image_lnx);
    test_module_register(&module_lib_base_zipcode);
}


/** \brief  Driver
 *
 * \param[in]   argc    argument count
 * \param[in]   argv    argument vector
 *
 * \return  EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
    cbmfm_log_set_level(CBMFM_LOG_DEBUG);
    cbmfm_errno = 0;

    register_modules();

    /* display usage message? */
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        usage();
        return EXIT_SUCCESS;
    }

    if (argc >= 2 && strcmp(argv[1], "--list-modules") == 0) {
        test_module_list_modules();
        return EXIT_SUCCESS;
    }

    if (argc >= 2 && strcmp(argv[1], "--list-tests") == 0) {
        test_module_list_tests(argc < 3 ? NULL : argv[2]);
        EXIT_SUCCESS;
    }

    /* now we can run tests */
    test_module_run_tests(argc < 2 ? NULL : argv[1],
            argc < 3 ? NULL : argv[2]);

    return EXIT_SUCCESS;
}



/** @} */
