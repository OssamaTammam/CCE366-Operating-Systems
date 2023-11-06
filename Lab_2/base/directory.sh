directoryPath="$1"

if [ -d "$directoryPath" ]; then
    echo "Directory already exists."
else
    echo "Directory does not exist. Creating it now..."
    mkdir -p "$directoryPath"
    echo "Directory created successfully."
fi

directoryInfoLast="directory-info.last"

# Check if the file or directory exists
if [ -e "$directoryInfoLast" ]; then
    echo "The file or directory '$directoryInfoLast' exists."
else
    # Create the file or directory
    touch "$directoryInfoLast"
    echo "Created the file or directory '$directoryInfoLast'."
fi
