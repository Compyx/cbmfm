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

/** \ingroup    tests
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lib.h"

#include "testcase.h"

/** \brief  Maximum number of modules that can be registerd
 */
#define MODULE_COUNT_MAX    64

/** \brief  List of registered modules
 */
static test_module_t *mod_list[MODULE_COUNT_MAX];

/** \brief  Number of registered modules
 */
static size_t mod_count = 0;



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


/** \brief  List tests in module \a name
 *
 * Lists tests in module \a name on stdout.
 *
 * \param[in]   name    test module name
 */
void test_module_list_tests(const char *name)
{
    const test_module_t *module;
    const test_case_t *test;
    size_t m;

    for (m = 0; m < mod_count; m++) {
        if (name == NULL || strcmp(mod_list[m]->name, name) == 0) {
            module = mod_list[m];
            printf("%s\t%s\n", module->name, module->desc);
            for (test = module->tests; test->name != NULL; test++) {
                printf("    %s\t%s\n", test->name, test->desc);
            }
        }
    }
}


/** \brief  Run tests in \a module
 *
 * Run one or more tests in module \a name. Use the \a name argument to specify a
 * single test, or pass either `NULL` or 'all' to run all tests.
 * Returns false on unexpected failures in the framework running the tests, not
 * if a test case fails.
 *
 * \param[in]   mod_name    test module (optional)
 * \param[in]   test_name   test name (optional)
 *
 * \return  bool
 */
bool test_module_run_tests(const char *mod_name, const char *test_name)
{
    test_module_t *module;
    test_case_t *test;
    size_t m;

    int total = 0;
    int failed = 0;

    for (m = 0; m < mod_count; m++) {
        if (mod_name == NULL || strcmp(mod_name, mod_list[m]->name) == 0) {
            module = mod_list[m];
            printf("\n\n. Running module '%s' (%s)\n",
                    module->name, module->desc);
            test_module_init(module);

            for (test = module->tests; test->name != NULL; test++) {
                if (test_name == NULL || strcmp(test_name, test->name) == 0) {
                    /* run test */
                    printf("\n\n... Running test '%s' (%s)\n",
                            test->name, test->desc);
                    test_case_init(test);
                    if (!(test->func(test))) {
                        /* unexpected failure */
                        test_module_exit(module);
                        return false;
                    }
                    module->total += test->total;
                    module->failed += test->failed;
                }
            }
            /* display results */
            printf("\n\n");
            if (module->failed == 0) {
                printf(". OK: %d tests, no failures -> 100%%\n", module->total);
            } else {
                printf(". OK: %d tests, %d failures -> %.2f%%\n",
                        module->total, module->failed,
                        (float)(module->total - module->failed)
                        / (float)(module->total) * 100.0);
            }
            total += module->total;
            failed += module->failed;
            test_module_exit(module);
        }
    }

    putchar('\n');
    if (failed == 0) {
        printf("\nFinal result: %d tests, no failures -> 100%%\n", total);
    } else {
        printf("\nFinal result: %d tests, %d failures -> %.2f%%\n",
                total, failed,
                (float)(total - failed) / (float)(total) * 100.0);
    }
    return true;
}


/** \brief  Register \a module
 *
 * This function calls exit() if #MODULE_COUNT_MAX is reached. In that case,
 * simple increase #MODULE_COUNT_MAX and recompile.
 *
 * \param[in]   module  test module
 */
void test_module_register(test_module_t *module)
{
    if (mod_count == MODULE_COUNT_MAX) {
        fprintf(stderr, "%s:%d:%s(): maximum number of modules reached.",
                __FILE__, __LINE__, __func__);
        exit(1);
    }
    mod_list[mod_count++] = module;
}


/** \brief  List registered modules
 */
void test_module_list_modules(void)
{
    size_t m;

    for (m = 0; m < mod_count; m++) {
        const test_module_t *module = mod_list[m];

        printf("%s\t%s\n", module->name, module->desc);
    }
}

/** @} */
