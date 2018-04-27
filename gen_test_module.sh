#!/bin/bash


# Show usage info
function usage()
{
    cat <<EOT
Usage: `basename $0` TEST_NAME

Where TEST_NAME is the path to the module being tested, with path separators
replaced with underscores and the 'src/' and '.c' stripped off.

For example: 'lib_base_io' will generate two files:
    src/tests/test_lib_base_io.c
    src/tests/test_lib_base_io.h
And generate a test_module_t object in the files called 'module_lib_base_io'.
EOT

}


# Entry point of script
if [ "$#" -eq 0 ] || [ "$1" = "--help" ] ; then
    usage
    exit 0
fi


# test name
TEST_NAME="$1"
# include guard to use in the test module .h file
TEST_GUARD="CBMFM_TEST_`tr a-z A-Z <<<${TEST_NAME}`_H"
# name of the .h file of the test module
HEADER_FILE="src/tests/test_${TEST_NAME}.h"
# name of the .c file of the test module
SOURCE_FILE="src/tests/test_${TEST_NAME}.c"
# header of tested module
MODULE_HEADER="`tr '_' '/' <<<${TEST_NAME}`.h"


# Generate .h file
echo -n "Generating ${HEADER_FILE} .. "
sed -e "s@TEST_NAME@${TEST_NAME}@g; s@TEST_HEADER_GUARD@${TEST_GUARD}@g" \
    data/templates/test_template.h > "$HEADER_FILE"
if [ $? -eq 0 ] ; then
    echo "OK"
else
    echo "failed"
    exit 1
fi


# Generate .c file
echo -n "Generating ${SOURCE_FILE} .. "
sed -e "s@TEST_NAME@${TEST_NAME}@g; s@MODULE_HEADER@${MODULE_HEADER}@g" \
    data/templates/test_template.c > "$SOURCE_FILE"
if [ $? -eq 0 ] ; then
    echo "OK"
else
    echo "failed"
    exit 1
fi

