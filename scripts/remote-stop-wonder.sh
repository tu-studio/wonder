 #!/bin/bash

SCRIPTDIR=`dirname $0`

${SCRIPTDIR}/stop-wonder.sh > /dev/null &

exit 0;
