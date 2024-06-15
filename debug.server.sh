#!/bin/bash
# Export the library path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/MsQuic/bin

#export LD_LIBRARY_PATH=/usr/lib/gtk-4.0/4.0.0/immodules:/usr/lib/gtk-4.0/4.0.0/media:/usr/lib/gtk-4.0/4.0.0/printbackends:$LD_LIBRARY_PATH

if ! gdb ./build/server; then
	echo "Error: Server debug failed"
	exit 1;
fi