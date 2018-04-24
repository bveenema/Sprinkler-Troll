#!/bin/bash
particle compile p --saveTo bash.bin
if [ $? -eq 0 ]; then
    echo OK
else
    echo FAIL
fi




# call particle compile p --saveTo o.bin
# if %ERRORLEVEL% == 0 goto :next
# echo "Errors encountered during execution.  Exited with status: %errorlevel%"
# goto :endofscript
#
# :next
# echo "Doing the next thing"
#
# :endofscript
# echo "Script complete"
