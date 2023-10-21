# backup.sh dir backupDir intervalSec maxBackups

# script paramters
dir="$1"
backupDir="$2"
maxBackups="$3"
currDirectory=$(pwd)
echo"$currDirectory"

createBackup() {
    # timestamp used to name the backup file
    timestamp=$(date +"%Y-%m-%d--%H-%M-%S")
    cp -r "$dir" "$backupDir/$timestamp"

    echo "Backup created at $timestamp"
}

checkDirectory() {
    directoryPath="$1"
    if [ -d "$currDirectory/$directoryPath" ]; then
        echo "Directory already exists."
    else
        echo "Directory does not exist. Creating it now..."
        mkdir -p "$currDirectory/$directoryPath"
        echo "Directory created successfully."
    fi
}

# Check if needed directories exist and if not create them
checkDirectory "$currDirectory/$dir"
checkDirectory "$currDirectory/$backupDir"
directoryInfoLast="$currDirectory/directory-info.last"

# Check if the file or directory exists
if [ -e "$directoryInfoLast" ]; then
    echo "The file or directory '$directoryInfoLast' exists."
else
    # Create the file or directory
    touch "$directoryInfoLast"
    echo "Created the file or directory '$directoryInfoLast'."
fi

# Check if files in the source directory have been modified
if find "$dir" -newer "directory-info.last" -print | grep -q .; then
    createBackup
    echo "Backup successful"

    # Delete oldest file if max number of backups reached
    numBackups=$(ls -1 "$currDirectory/$backupDir" | wc -l)
    if [ "$numBackups" -gt "$maxBackups" ]; then
        oldestBackup=$(ls -t -1 "$currDirectory/$backupDir" | tail -1)

        # Remove the oldest backup
        rm -rf "$currDirectory/$backupDir/$oldestBackup"
        echo "Deleted oldest backup: $oldestBackup"
    fi

    # Update the reference file
    touch "directory-info.last"
fi
