#!/bin/sh
# Render all polyhedral net projections.
# Usage: sh test/render_nets.sh

set -e

SCRIPT_DIR=`dirname "$0"`
RENDER="uv run $SCRIPT_DIR/render_net.py"

$RENDER --proj="+proj=tsea +R=1" \
    --title="TSEA: Tetrahedral Snyder Equal Area" \
    -o tsea_net.png
