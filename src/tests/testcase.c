/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/testcase.c
 * \brief   Unit testing module
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
#include <stdbool.h>
#include <string.h>

#include "lib.h"

#include "testcase.h"


/** \brief  Initialize \a test
 *
 * Sets counters to 0.
 *
 * \param[in,out]   test    test case
 */
static void test_case_init(test_case_t *test)
{
    test->total = 0;
    test->failed = 0;
}


/** \brief  Initialize \a module
 *
 * Sets counters to 0 and calls the setup function if that is declared.
 *
 * \param[in,out]   module  test module
 */
void test_module_init(test_module_t *module)
{
    module->total = 0;
    module->failed = 0;

    if (module->setup != NULL) {
        module->setup();
    }
}


/** \brief  Exit \a module
 *
 * Calls the teardown function if that is declared.
 *
 * \param[in,out]   module  test module
 */
void test_module_exit(test_module_t *module)
{
    if (module->teardown != NULL) {
        module->teardown();
    }
}


/** \brief  List tests in \a module
 *
 * Lists tests in \a module on stdout: name\<tab\>description
 *
 * \param[in]   module  test module
 */
void test_module_list_tests(const test_module_t *module)
{
    const test_case_t *t;

    for (t = module->tests; t != NULL; t++) {
        printf("%s\t%s\n", t->name, t->desc);
    }
}


/** \brief  Run tests in \a module
 *
 * Run one or more tests in \a module. Use the \a name argument to specify a
 * single test, or pass either `NULL` or 'all' to run all tests.
 * Returns false on unexpected failures in the framework running the tests, not
 * if a test case fails.
 *
 * \param[in,out]   module  test module
 * \param[in]       name    test name (optional)
 *
 * \return  bool
 */
bool test_module_run_tests(test_module_t *module, const char *name)
{
    test_case_t *test;

    test_module_init(module);

    for (test = module->tests; test != NULL; test++) {
        if (name == NULL || strcmp(name, test->name) == 0) {
            /* run test */
            test_case_init(test);
            if (!(test->func())) {
                /* unexpected failure */
                return false;
            }
            module->total += test->total;
            module->failed += test->failed;
        }
    }

    test_module_exit(module);
    return true;
}

