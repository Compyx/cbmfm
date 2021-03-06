/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/testcase.h
 * \brief   Unit testing module - header
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

#ifndef CBMFM_TESTS_TESTCASE_H
#define CBMFM_TESTS_TESTCASE_H

#include <stdbool.h>


/** \brief  Test case object
 *
 * Holds information on a test. A test can do multiple 'sub tests' if required.
 */
typedef struct test_case_s {
    const char *name;   /**< test name, used on the command line */
    const char *desc;   /**< test description */
    bool (*func)(struct test_case_s *test);  /**< function to run the test */
    int total;          /**< total number of subtests */
    int failed;         /**< number of failed subtests */
} test_case_t;


/*
 * Note: For some reason the @{ @} stuff doesn't work to declare the following
 *       struct part of the Doxygen group 'tests', so a manual \ingroup is
 *       required. Most likely a Doxygen bug.
 */

/** \brief  Test module object
 *
 * \ingroup tests
 *
 * A module contains a list of tests to run, these can be run consecutive, or
 * a single test can be selected on the command line.
 *
 * The setup() function can be used to set up some resources before running the
 * tests (opening a disk image for example), the teardown() function can be
 * used to clean up resources initialized by the setup() function.
 */
typedef struct test_module_s {
    const char *name;           /**< module name */
    const char *desc;           /**< module description */
    test_case_t *tests;         /**< list of tests */
    bool (*setup)(void);        /**< optional setup function */
    void (*teardown)(void);     /**< optional teardown function */
    int total;                  /**< total number of tests performed */
    int failed;                 /**< number of failures */
} test_module_t;


void test_module_register(test_module_t *module);
bool test_module_init(test_module_t *module);
void test_module_exit(test_module_t *module);
void test_module_list_tests(const char *name);
void test_module_list_modules(void);
bool test_module_run_tests(const char *mod_name, const char *test_name);

/** @} */
#endif
