#	filename	Makefile
# desc	ofp adapter Makefile(mount -t nfs -o nolock 10.0.0.250:/opt/nfs /mnt/nfs)
# contact  pioevh@163.com
# date  20160809

################################
##gcc define
################################
CROSS_COMPILE=powerpc-e300c3-linux-gnu-
CROSS_COMPILE=

################################
##common
################################
appname=ofpa
objects=main
outpath=/opt/nfs/
CC_OPT=-Wall -v -g -H -m32
CC=$(CROSS_COMPILE)gcc $(CC_OPT)
$(appname):$(objects).o
	$(CC) $(objects).o -o $(outpath)$(appname)
$(objcts):$(objects).c $(appname).c ptnapilib.c ofplib.c

.PHONY:clean
clean:
	-rm -rf $(outpath)$(appname) *.o *.gch $(objects)
