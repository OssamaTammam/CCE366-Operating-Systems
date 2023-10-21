# backup.sh dir backupDir maxBackups

# script paramters
dir="$1"
backupDir="$2"
maxBackups="$3"
currDirectory=$(pwd)

dir="$currDirectory/$dir"
backupDir="$currDirectory/$backupDir"
directoryInfoLast="$currDirectory/directory-info.last"

createBackup() {
    # timestamp used to name the backup file
    timestamp=$(date +"%Y-%m-%d--%H-%M-%S")
    cp -r "$dir" "$backupDir/$timestamp"

    echo "Backup created at $timestamp"
}

checkDirectory() {
    directoryPath="$1"
    if [ -d "$directoryPath" ]; then
        echo "Directory already exists."
    else
        echo "Directory does not exist. Creating it now..."
        mkdir -p "$directoryPath"
        echo "Directory created successfully."
    fi
}

createInfoFile() {
    if [ -e "$directoryInfoLast" ]; then
        echo "The file or directory '$directoryInfoLast' exists."
    else
        # Create the file or directory
        touch "$directoryInfoLast"
        echo "Created the file or directory '$directoryInfoLast'."
    fi
}

# Check if needed directories exist and if not create them
checkDirectory "$dir"
checkDirectory "$backupDir"
createInfoFile

# Check if files in the source directory have been modified
if find "$dir" -newer "$directoryInfoLast" -print | grep -q .; then
    createBackup
    echo "Backup successful"

    # Delete oldest file if max number of backups reached
    numBackups=$(ls -1 "$backupDir" | wc -l)
    if [ "$numBackups" -gt "$maxBackups" ]; then
        oldestBackup=$(ls -t -1 "$backupDir" | tail -1)

        # Remove the oldest backup
        rm -rf "$backupDir/$oldestBackup"
        echo "Deleted oldest backup: $oldestBackup"
    fi

    # Update the reference file
    touch "$directoryInfoLast"
fi
