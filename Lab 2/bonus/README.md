# Lab 2 Bonus
## Overview
Simple cronjob shell script that backs up directories periodically 

## Folder Hierarchy
    backup-cron.sh
    Shell script that checks for directories and then backs up the source directory

## Prerequisite
1. Install make using the following command
   ```bash
   sudo apt install make
   ```
2. Open the terminal in the directory and run the following command
   ```bash
   chmod +x backup-cron.sh
   ```

## Setup
1. Open a terminal and navigate to the directory you want the files to be created
2. Use the following command ```crontab -e``` to open the crontab window
3. Add the script absolute path to the cron file ex.
   ```
   * * * * * /absolute/path/to/backup-cron.sh ${NAME_OF_SOURCE_DIRECTORY} ${NAME_OF_BACKUP_DIRECTORY} ${MAX_NUMBER_OF_BACKUPS}
   ```
   EX. I need to run this backup every 3rd Friday
of the month at 12:31 am
   ```
   31 0 * * 5#3 /absolute/path/to/backup-cron.sh ${NAME_OF_SOURCE_DIRECTORY} ${NAME_OF_BACKUP_DIRECTORY} ${MAX_NUMBER_OF_BACKUPS}
   ```
4. Save the cron file
5. If it didn't work right away use the following command to restart all cron jobs ```sudo systemctl restart cron```