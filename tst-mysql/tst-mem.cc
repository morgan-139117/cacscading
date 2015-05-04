/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <iostream>
#include <string>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

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
std::cout<<"c";

		while(cur != NULL){
			if(cur->value != NULL)
			std::cout<<cur->value;
			cur = cur->next;
		}
std::cout<<"c";
	}

};

void newmem(LeakingList& charList, long size) {
	std::string name;


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
	printf("resource pressure %f ", rp);



}

int main(int ac, char** av)

{
	microseconds = 50000000;
	LeakingList charList;
	long size = 1;
	for (int i = 0; i < 800;i++) {
		newmem(charList, size);
		size += 10240;
	}

	for (int i = 0; i < 100;i++) {
		usleep(microseconds);
		charList.show();
		
	}

	return 0;
}

