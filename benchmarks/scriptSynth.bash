#!/bin/bash

blifDir=$HOME/tools/mvsis/mvsis-1.3/examples/ex/comb/mcnc91/mlex
genlibDir=$HOME/tools/sis/sis-1.3.6/sis/sis_lib
genlibScript=script.myMcnc

for bench in `ls ${blifDir}/*\.blif`; do
	bench=`echo ${bench} | sed 's%.*/\(.*\)\.blif%\1%g'`
	benchDir=comb	
	
	echo "read_blif ${blifDir}/${bench}.blif
	      source ${genlibDir}/${genlibScript}
	      write_bdnet ${benchDir}/${bench}.bdnet
	      quit" | sis >> synth.log
done
