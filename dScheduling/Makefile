CC = /home/inesvdm/buildroot/buildroot-2021.08/output/host/bin/aarch64-buildroot-linux-uclibc-g++
  
CFLAGS = -g
RM = /bin/rm -f
objects = dScheduledTimes

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -C $(KDIR) -o dScheduledTimes dScheduledTimes.cpp ../CScheduling.cpp
clean:
	${RM} *.o dScheduledTimes