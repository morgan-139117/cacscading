/*
Dell 5547. 4G memory. Ubuntu 14.04 x64. Swappiness = 95 lol

Use swap file :

1. dd if=/dev/zero of=/media/fasthdd/swapfile.img bs=1024 count=1M

2. mkswap /media/fasthdd/swapfile.img

3. # Add this line to /etc/fstab 
   /media/fasthdd/swapfile.img swap swap sw 0 0

4. swapon /media/fasthdd/swapfile.img

Use Osv Test case 

1. set command line to Image: ./scripts/run.py -e "tests/tst-mem.so 100"  

2. launch OSv with 1G mem:

 qemu-system-x86_64  -m 1G -smp 4 -device virtio-blk-pci,id=blk0,bootindex=0,drive=hd0,scsi=off -drive file=/home/abc/osv7/build/release/usr.img,if=none,id=hd0,aio=native,cache=none -netdev user,id=un0,net=192.168.122.0/24,host=192.168.122.1 -device virtio-net-pci,netdev=un0  -device virtio-rng-pci -enable-kvm -cpu host,+x2apic -chardev stdio,mux=on,id=stdio,signal=off -mon chardev=stdio,mode=readline,default -device isa-serial,chardev=stdio

Larger than 1G recive following error when launching 4 instances
Cannot set up guest memory 'pc.ram': Cannot allocate memory

Result

As 3 out of 4G memory was available, each machine held 1G in mem when three instances were launched. 

After all 4 instances were launched, memory requests exceeded the capacity, large part of memory held by first two instanced were swapped out.

And Swap was full.



*/


#include <iostream>
#include <string>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sstream>
unsigned int microseconds;

struct sysinfo myinfo;
unsigned long total_bytes;
unsigned long freeram_bytes;



class Node {
public:
	Node(char* abc) { 
		value = abc;
		next = NULL;
	}
	Node* next;
	char* value;
};

class LeakingList {


private:
	Node* head;
	Node* cur;
	int lsize;

public:

	int size(){
		return this->lsize;
	}

	LeakingList() {
		head = new Node(NULL);
		cur = head;
		lsize = 0;
	}
	void add(char* leak) {
		lsize++;
		cur->next = new Node(leak);
		cur = cur->next;
	}

	void show(){
		cur = head;
		std::cout<<"showed";
		int i = 0;
		while(cur != NULL){
			if(cur->value != NULL)
			i++;			
			//std::cout<<cur->value;
			cur = cur->next;
		}

	}

};

void newmem(LeakingList& charList, long size) {

	char* largechar = new char[size]; // have to cheat the compiler this way lol

	for(int i = 0;i < size; i++){

			largechar[i] = 'a' ;

		}

	charList.add(largechar);

	sysinfo(&myinfo);

	total_bytes = myinfo.mem_unit * myinfo.totalram;
	printf("total usable main memory is %lu MB, %lu MB\n",
			freeram_bytes / 1024 / 1024, total_bytes / 1024 / 1024);

	float rp = sqrt(1 - (float) freeram_bytes / (float) total_bytes);

	freeram_bytes = myinfo.mem_unit * myinfo.freeram;
	printf("resource pressure %f , lsize %d", rp,charList.size());

}

int main(int ac, char** av)

{

   	double limit = 0.0;

		std::istringstream ss(av[1]);

		ss >> limit;


	microseconds = 5000000;
	LeakingList charList;
	long size = 10240240l;
	for (int i = 0; i < (int)limit;i++) {
		newmem(charList, size);
	}

	for (int i = 0; i < 30;i++) {
		usleep(microseconds);
		charList.show();
		getchar();
	}
	

	return 0;
}
