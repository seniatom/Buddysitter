CC = /home/inesvdm/buildroot/buildroot_base/buildroot-2021.08/output/host/bin/aarch64-buildroot-linux-uclibc-g++
INCLUDES = -I/home/inesvdm/buildroot/buildroot_base/buildroot-2021.08/output/host/include/python3.9/
INC = $(INCLUDES) -lpython3.9

CFLAGS = -g
RM = /bin/rm -f
objects = main 



all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) $(INC) -C $(KDIR) -o BuddySitter main.cpp CScheduling.cpp CMotor.cpp CSpeaker.cpp CStreaming.cpp CHX711.cpp CDatabaseHandler.cpp
clean:
	${RM} *.o BuddySitter
