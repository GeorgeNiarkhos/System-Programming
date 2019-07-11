#!/bin/bash
rfilecnt=0
wfilecnt=0
rbytescnt=0
wbytescnt=0
incnt=0
endcnt=0
while read -a LINE
	do
		if [ "${LINE[0]}" == "START" ];then
			idarray[$incnt]=${LINE[1]}
			incnt=$((incnt+1))
		fi
		if [ "${LINE[0]}" == "END" ];then
			endcnt=$((endcnt+1))
		fi
		if [ "${LINE[0]}" == "FILE" ];then
			if [ "${LINE[1]}" == "READ" ];then
				rfilecnt=$((rfilecnt+1))
			fi 
			if [ "${LINE[1]}" == "WRITTEN" ];then
				wfilecnt=$((wfilecnt+1))
			fi
		fi
		if [ "${LINE[0]}" == "READ" ];then
			rbytescnt=$((rbytescnt+${LINE[1]}))
		fi
		if [ "${LINE[0]}" == "WROTE" ];then
			wbytescnt=$((wbytescnt+${LINE[1]}))
		fi
	done
echo "$incnt Clinets have conected with ids:"

for((i=0; i <= ${#idarray[@]} - 1; i++))
do
	if [ $i -eq 0 ];then
		minid=${idarray[0]}
		maxid=${idarray[0]}
	else
		if [ $minid -gt ${idarray[$i]} ];then
			minid=${idarray[$i]}
		fi
		if [ ${idarray[$i]} -gt $maxid ];then
			maxid=${idarray[$i]}
		fi
	fi
	echo "${idarray[$i]}"
done
echo "MinId $minid"
echo "MaxId $maxid"
echo "$rbytescnt bytes read"
echo "$wbytescnt bytes written"
echo "$rfilecnt files read"
echo "$wfilecnt files written"
echo "$endcnt clients left"
