#!/bin/bash

help() {
    echo "$0 [-j jobs]"
    printf " -j \t The amount of jobs to use for make on lua.\n"
}

check_for_command() {
    COMMAND=$1
    MESSAGE=$2

    if ! type $COMMAND > /dev/null; then
        >&2 echo $MESSAGE
    fi
}

JOBS=$(nproc)

while getopts "h?j:" opt; do
    case "$opt" in
        h|\?)
            help
            exit 0
            ;;
        j)
            JOBS=$OPTARG
            ;;
        *)
            help
            exit 0
            ;;
    esac
done

shift $(($OPTIND-1))

[ "${1:-}" = "--" ] && shift

echo "Building using $JOBS jobs. $@"

exit 0

check_for_command make "Make is required to build lua using the bootstrap script."

echo "Building Lua"

pushd lua-5.4.7/

make

popd

echo "Finished building lua"
