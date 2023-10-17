# backup.sh dir backupDir intervalSec maxBackups

# script paramters
dir="$1"
backupDir="$2"
intervalSec="$3"
maxBackups="$4"

createBackup() {
    # timestamp used to name the backup file
    timestamp=$(date +"%Y-%m-%d--%H-%M-%S")
    cp -r "$dir" "$backupDir/$timestamp"

    echo "Backup created at $timestamp"
}

while true; do
    sleep "$intervalSec"

    # Check if files in the source directory have been modified
    if find "$dir" -newer "directory-info.last" -print | grep -q .; then
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
        touch "directory-info.last"
    fi
done
