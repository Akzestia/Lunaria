export LD_LIBRARY_PATH=/usr/lib/gtk-4.0/4.0.0/immodules:/usr/lib/gtk-4.0/4.0.0/media:/usr/lib/gtk-4.0/4.0.0/printbackends:$LD_LIBRARY_PATH

# Run the compiled client
if ! ./build/client; then
    echo "Error: Client execution failed"
    exit 1
fi