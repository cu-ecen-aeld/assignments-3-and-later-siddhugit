#!/bin/bash

usage(){
    printf "Usage: ./finder.sh <dir_name> <search_string>\n"
}

if [ $# -ne 2 ]; then
    printf "ERROR: Number of arguments=%d, it should be 2\n" $#
    usage
    exit 1
fi

filesdir=$1
searchstr=$2

if [ ! -d ${filesdir} ];then
    printf "ERROR: %s is not a directory\n" "${filesdir}"
    exit 1
fi

numFiles=$(find "${filesdir}" -type f 2>/dev/null | wc  -l)
numMatches=$(grep -r "${searchstr}" "${filesdir}" 2>/dev/null | wc  -l)

printf "The number of files are %d and the number of matching lines are %d\n" ${numFiles} ${numMatches}

exit 0
