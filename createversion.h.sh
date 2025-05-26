#/bash/sh
set -e

#Read version string
version=$(head -n 1 $1)
version=$(echo $version | xargs echo -n)
branch=$(git branch --show-current)
branch=$(echo $branch | xargs echo -n)

#Debug parameters
#echo "$ 1="$1
#echo "$ 2="$2

#Write header file
echo "#pragma once" > $2
echo "" >> $2
echo "#define APP_VERSION \""$version"-"$branch"\"" >> $2
echo "#define APP_MAJOR_VERSION "$(cut -d'.' -f1 <<<$version) >> $2
echo "#define APP_MINOR_VERSION "$(cut -d'.' -f2 <<<$version) >> $2
echo "#define APP_PATCH_VERSION "$(cut -d'.' -f3 <<<$version) >> $2
echo "#define APP_BRANCH \""$branch"\"" >> $2

version+="-"
version+=$branch
echo $version "-> version.h"
