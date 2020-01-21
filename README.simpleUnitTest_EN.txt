simpleUnitTest.sh: minimal "black box" (or "external") "unit" test script.

########################################
# Creation August 2012
# Update August 2013
########################################
# No restriction on usage nor dissemination
########################################
# Problems, comments: nicolas.castagne@phelma.grenoble-inp.fr
########################################

########################################
# Rapid launch of tests for increment 1 of the MIPS SIMULATOR project
########################################
1 / place the executable to be tested in the figure, for example the interpreter directory
2 / enter, for example
../testing/simpleUnitTest.sh -e ./emulMips test / exit / *. emu

Or :
../testing/simpleUnitTest.sh is the path to test script
./emulMips is the path to the executable to test (the Mips emulator here)
test / exit / *. emu designates all the files of the 'exit' command of the simulator to be tested

To launch in batch mode: -b option:
../testing/simpleUnitTest.sh -e ./emulMips -b test / exit / *. emu


See below for details.

########################################
# Introduction
########################################
The script launches an <executable> on a list of files from <test>, the whole being passed as an argument to the script.


Each <test> .emu file must contain:
1 / emulator commands (one per line)
2 / possibly comments (everything after #)
3 / in the first line, the variable # TEST_RETURN_CODE = X or X is in {FAIL, PASS}
4 / possibly in second line # TEST_COMMENT = "Test standard operation" which will be displayed during the tests


TEST_RETURN_CODE, which values ​​should be
PASS if the test should pass without an error code (zero)
FAIL if the test should generate an error code (non zero)

TEST_COMMENT (optional)
a comment string describing the test


Example: here is a valid header in a test file:
# -------
TEST_RETURN_CODE = PASS # the test should pass w / o error
TEST_COMMENT = "Test empty file" # this comment will be displayed before performing the test
# -------


For each of the <test> files passed as arguments, the script:
- launch the <executable> by passing it <test> as an argument
- detects if the <executable> has "crashed" (segmentation fault, etc.)
- detects if the exit code returned by the <executable> corresponds to the expected error code (in accordance with

Finally, the script generates a test report with the results of all the tests.

For options and help type:
  ../testing/simpleUnitTest.sh -h

