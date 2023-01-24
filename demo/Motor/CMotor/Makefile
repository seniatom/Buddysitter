CC = /home/inesvdm/buildroot/buildroot-2021.08/output/host/bin/aarch64-buildroot-linux-uclibc-g++
  
CFLAGS = -g
RM = /bin/rm -f
objects = CMotor

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -o CMotor CMotor.cpp -w
clean:
	${RM} *.o CMotor