#!/bin/bash


copy_dependencies() {
    exe="$1"   # First argument: the executable
    dest="$2"  # Second argument: the destination directory

    if [ -z "$exe" ] || [ ! -f "$exe" ]; then
        echo "Error: Executable not specified or does not exist."
        return 1
    fi

    # Create the destination directory if it doesn't exist
    mkdir -p "$dest"

    # Use ldd to list dependencies and filter for valid paths
    deplist=$(ldd "$exe" | awk '{if (match($3,"/")){printf("%s "),$3}}')

    if [ -z "$deplist" ]; then
        echo "No dependencies found or ldd command failed."
        return 1
    fi

    # Copy each dependency to the destination
    echo "Copying dependencies of $exe to $dest..."
    for lib in $deplist; do
        cp "$lib" "$dest"
    done

    echo "Dependencies copied successfully."
}

cd build_output/app/x86
mkdir libs
copy_dependencies "ReadSTL" "libs"
