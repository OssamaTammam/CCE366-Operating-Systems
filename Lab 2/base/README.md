# Lab 2
## Overview
Simple shell script that backs up directories periodically 

## Folder Hierarchy
    backup.sh
        Shell script that handles backing up the files
    directory.sh
        Shell script that manages creating directories
    MAKEFILE
        Makefile that takes in parameters and runs scripts

## Prerequisite
1. Open the terminal in the directory
2. Run the following commands
   ```bash
   chmod +x backup.sh
   chmod +x directory.sh
   ```

## Usage
1. Navigate in the terminal to where the script is located
2. Run the following bash command
   ```bash
   make -f MAKEFILE run SOURCE_DIRECTORY=${NAME_OF_SOURCE_DIRECTORY} BACKUP_DIRECTORY=${NAME_OF_BACKUP_DIRECTORY} INTERVAL=${TIME_INTERVAL} MAX_BACKUPS=${MAX_BACKUPS}

   ```
3. Press CTRL+C in the terminal to terminate program