CC = /home/inesvdm/buildroot/buildroot-2021.08/output/host/bin/aarch64-buildroot-linux-uclibc-g++
  
CFLAGS = -g
RM = /bin/rm -f
objects = main CScheduling CWeightSensor CMotor CSpeaker CStreaming CDatabaseHandler

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -C $(KDIR) -o BuddySitter main.cpp CScheduling.cpp CMotor.cpp CSpeaker.cpp CStreaming.cpp CWeightSensor.cpp CDatabaseHandler.cpp
clean:
	${RM} *.o BuddySitter