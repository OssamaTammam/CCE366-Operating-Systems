directoryPath="$1"

if [ -d "$directoryPath" ]; then
    echo "Directory already exists."
else
    echo "Directory does not exist. Creating it now..."
    mkdir -p "$directoryPath"
    echo "Directory created successfully."
fi
