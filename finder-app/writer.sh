#!/bin/bash

usage(){
    printf "Usage: ./writer.sh <file_name> <text_to_write>\n"
}

if [ $# -ne 2 ]; then
    printf "ERROR: Number of arguments=%d, it should be 2\n" $#
    usage
    exit 1
fi

writefile=$1
writestr=$2

fileDir=$(dirname "${writefile}")

mkdir -p "${fileDir}"

if  ! printf "${writestr}\n" > "${writefile}"; then
    printf "Error writting to ${writefile}\n"
    exit 1
fi

exit 0