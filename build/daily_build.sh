#!/bin/sh
date
if [ "$1" = "P01001" ] || [ "$1" = "P01002" ] || [ "$1" = "P01003" ] || [ "$1" = "P02001" ] || [ "$1" = "P02002" ] || [ "$1" = "P02003" ]; then
	TAG="Daily_`date +%Y%m%d`_"$1
else
	echo "========================================================================="
        echo "Please use"
        echo "./build/daily_build.sh branchname"
        echo "Vaild branchname are P02003"
        echo "========================================================================="
        exit
fi

#Get source code
#repo init -u git://wld6.wnc.com.tw/platform/manifest.git -b Telenor -m manifest.xml --repo-url=git://wld6.wnc.com.tw/tools/repo.git

echo "checkout master..."
git checkout master
#if [ "$1" = "P01001" ] || [ "$1" = "P02001" ]; then
	echo "clean the project..."
	git clean -df
#fi
echo "reset the project..."
git reset --hard
echo "sync the project..."
git pull

echo "=============== start to build UGW ... ================="
echo DWR-966_`date +%Y%m%d`>ugw_version

#Change config
#./build/boardcfg.sh
./scripts/ltq_change_environment.sh switch ./config/$1

#time make V=s
time make -j 4
echo "=============== build UGW DONE!! ================="

git push origin HEAD:refs/tags/$TAG
date



#if [ "$1" = "P02001" ] || [ "$1" = "P02002" ] || [ "$1" = "P02003" ]; then
#	echo "==============start to build WLD7 NB UGW... =========="
#	sed -i "s/\/\/#define __NEW_PCB_CONFIG__/#define __NEW_PCB_CONFIG__/" kernel/arch/mips/infineon/ar10/ar10_ref_board.c
#	sed -i "s/\/\/#define __NEW_PCB_CONFIG__/#define __NEW_PCB_CONFIG__/" kernel/drivers/leds/ifxmips_led.c
#	./build/boardcfg.sh wld7 l2
#	./scripts/ltq_change_environment.sh switch ./config/$1
#	echo WLD7_`date +%Y%m%d`_NB>ugw_version
#	make
	echo "==============start to build WLD9 UGW DONE!!... =========="
#fi

