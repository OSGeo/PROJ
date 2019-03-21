#!/bin/bash

# All platform-specific before_install scripts starts by running this
# "global" before_install script.

set -e

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    # Workaround apt-get update issue (can be removed later)
    sudo rm -f /etc/apt/sources.list.d/cassandra*
fi
