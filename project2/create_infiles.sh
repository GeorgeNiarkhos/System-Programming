#!/bin/bash
dirname=$1
numoffiles=$2
numofdirs=$3
tmpndirs=$3
levels=$4
tmplvls=$4
if [ "$#" -ne 4 ];then
	echo "Illegal number of arguments"
else
	if (( [[ $numoffiles =~ ^-?[0-9]+$ ]] && [[ $numoffiles -ge 0 ]] ) && ( [[ $numofdirs =~ ^-?[0-9]+$ ]] && [[ $numofdirs -ge 0 ]] ) && ( [[ $levels =~ ^-?[0-9]+$ ]] && [[ $levels -ge 0 ]] ));then #valid vars
		rm -rf "$dirname"
		mkdir "$dirname"
		path="./$dirname"
		declare -A patharray
		patharray[0]="$path"
		for ((i=1; i <= $numofdirs; i++))
		do
			ransize=$(( ( RANDOM % 8 )  + 1 ))
			# echo "$ransize"
			randir=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $ransize | head -n 1)
			# echo "$randir"
			if [ $tmplvls -ne 0 ];then
				path="$path/$randir"
				mkdir "$path"
				tmplvls=$((tmplvls-1))
				patharray[$i]="$path"
				# echo "${patharray[$i]} hohohoho"

			else
				path="./$dirname"
				path="$path/$randir"
				mkdir "$path"
				tmplvls=$4
				patharray[$i]="$path"
				# echo "${patharray[$i]} hohohoho"
			fi
		done
		tmpndirs=0
		# echo "------"
		for ((i=0; i < $numoffiles; i++))
		do
			ransize=$(( ( RANDOM % 8 )  + 1 ))
			ranname=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $ransize | head -n 1)
			# echo "${patharray[$tmpndirs]} $tmpndirs $numofdirs"
			fname="${patharray[$tmpndirs]}/$ranname"
			# echo "$fname"
			touch $fname
			ransize=$(( ( RANDOM % 128 )  + 1 ))
			ransize=$((ransize  * 1024 ))
			(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $ransize | head -n 1) >$fname
			tmpndirs=$((tmpndirs+1))
			if [ $tmpndirs -gt $numofdirs ];then
				tmpndirs=0
			fi
		done
	else
		echo "Invalid arguments"
	fi
fi