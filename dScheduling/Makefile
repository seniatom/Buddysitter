CC = /home/inesvdm/buildroot/buildroot_base/buildroot-2021.08/output/host/bin/aarch64-buildroot-linux-uclibc-g++
INCLUDES = -I/. -I/home/inesvdm/buildroot/buildroot_base/buildroot-2021.08/output/host/include/python3.9/
INC = $(INCLUDES) -lpython3.9

CFLAGS = -g
RM = /bin/rm -f
objects = dScheduledTimes

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS)  $(INC) -C $(KDIR) -o dScheduledTimes dScheduledTimes.cpp ../CScheduling.cpp ../CDatabaseHandler.cpp
clean:
	${RM} *.o dScheduledTimes