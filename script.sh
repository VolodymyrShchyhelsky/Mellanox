#!/bin/bash

time_add () #old-h old-m old-s diff-h diff-m diff-s
{
  let "$1 = $1 + $4"
  let "$2 = $2 + $5"
  let "$3 = $3 + $6"

  if [[ $3 -ge 60 ]]; then
    let "$3 = $3 - 60"
    let "$2 = $2 + 1"
  fi

  if [[ $2 -ge 60 ]]; then
    let "$2 = $2 - 60"
    let "$1 = $1 + 1"
  fi

  if [[ $1 -ge 24 ]]; then
    let "$1 = $1 - 1"
  fi
}

diff_time () #old-h old-m old-s new-h new-m new-s diff-h diff-m diff-s
{
  let "$7 = $4 - $1"
  let "$8 = $5 - $2"
  let "$9 = $6 - $3"
  
  if [[ $9 -lt 0 ]]; then
    let "$9 = $9 + 60"
    let "$8 = $8 - 1"
  fi

  if [[ $8 -lt 0 ]]; then
    let "$8 = $8 + 60"
    let "$7 = $7 - 1"
  fi
}

dmesg -T | sed -r 's/[0-9]{1,4}]/YEAR]/g' | sed -r 's/\[([^\s]+\s+[^\s]+\s+[0-9]{1,2}\s)/\[The Log 01 /g' > log1.txt

sleep 1

dmesg -T | sed -r 's/[0-9]{1,4}]/YEAR]/g' | sed -r 's/\[([^\s]+\s+[^\s]+\s+[0-9]{1,2}\s)/\[The Log 02 /g' > log2.txt

file=`pwd`/log1.txt

log1=$(cat "$file")

last_line=""

while IFS='\n' read -ra ADDR; do
  if [[ ${ADDR[0]} = *"["* ]]; then
    last_line=${ADDR[0]}
  fi
done <<< "$log1"

IFS=' '
read _1 _2 _3 time _4 <<< $last_line

IFS=':'
read start_h start_m start_s <<< $time

#delete leading zeros
start_h=${start_h#0}
start_m=${start_m#0}
start_s=${start_s#0}

time_add start_h start_m start_s 1 2 4

touch log3.txt

IFS=$'\n'
while IFS=$'\n' read -ra ADDR; do
  buffer=""
  for i in ${ADDR[@]}; do
    if [[ -z buffer ]]; then
      buffer=$i
    else
      buffer="$buffer $i"
    fi
  done
  echo "$buffer" >> log3.txt
done <<< "$log1"

file1=`pwd`/log2.txt

log2=$(cat "$file1")

prev_h=-1
prev_m=-1
prev_s=-1

while IFS=$'\n' read -ra ADDR; do
  buffer=""
  for i in ${ADDR[@]}; do
    if [[ $i = *"["* ]]; then
      IFS=' '
      read _1 _2 _3 time _4 <<< $i

      IFS=':'
      read hour minute second <<< $time
     
      #delete leading zeros
      hour=${hour#0}
      minute=${minute#0}
      second=${second#0}

      if [[ prev_h -gt -1 ]]; then
        diff_h=0
        diff_m=0
        diff_s=0

        diff_time prev_h prev_m prev_s hour minute second diff_h diff_m diff_s
        
        time_add start_h start_m start_s diff_h diff_m diff_s
      fi

      printf -v disp_h "%02d" $start_h
      printf -v disp_m "%02d" $start_m
      printf -v disp_s "%02d" $start_s

      i="$_1 $_2 $_3 ${disp_h}:${disp_m}:${disp_s} $_4"

      IFS=$'\n'
    fi
    if [[ -z buffer ]]; then
      buffer=$i
    else
      buffer="$buffer $i"
    fi
  done
  prev_h=$hour
  prev_m=$minute
  prev_s=$second
  echo "$buffer" >> log3.txt
done <<< "$log2"

