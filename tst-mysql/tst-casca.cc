
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <atomic>
#include "osv/sched.hh"
#include <sys/sysinfo.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

typedef std::map<std::string, std::string> TStrStrMap;
typedef std::pair<std::string, std::string> TStrStrPair;


unsigned int microseconds;

struct sysinfo myinfo;
unsigned long total_bytes;
unsigned long freeram_bytes;


sched::profilem pft1;
sched::profilem pft2;
sched::profilem pfleaking;


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
		int a = 0;
		while(cur != NULL){
			if(cur->value != NULL)
				a++;
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
/*
	total_bytes = myinfo.mem_unit * myinfo.totalram;
	printf("total usable main memory is %lu MB, %lu MB\n",
			freeram_bytes / 1024 / 1024, total_bytes / 1024 / 1024);

	float rp = sqrt(1 - (float) freeram_bytes / (float) total_bytes);

	freeram_bytes = myinfo.mem_unit * myinfo.freeram;
	printf("resource pressure %f , lsize %d", rp,charList.size());*/
}



int main() {

	LeakingList charList;

	microseconds = 500000;

	std::thread t1([&] {
		// Thread with priority 1
	//	sched::set_adj_set();

			for ( int i=0; i<600; i++) {
				auto now = osv::clock::uptime::now();
				std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
				pft1.insert(TStrStrPair("misc t1 =>"+tmpnow,tmpnow));

				//	 std::cout << 1 << std::endl;
				for ( int j=0; j<30000000; j++) {


					// To force gcc to not optimize this loop away
					asm volatile("" : : : "memory");
				}
			}

		});

	std::thread t2([&] {
			// Thread with priority 1
	//	sched::set_adj_set();

				for ( int i=0; i<600; i++) {
					auto now = osv::clock::uptime::now();
					std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
					pft2.insert(TStrStrPair("misc t2 =>"+tmpnow,tmpnow));

					//	 std::cout << 1 << std::endl;
					for ( int j=0; j<30000000; j++) {


						// To force gcc to not optimize this loop away
						asm volatile("" : : : "memory");
					}
				}

			});




	std::thread leaking([&] {
					// Thread with priority 1
		//	sched::set_adj_set();
			long size = 10240240l;
						for ( int i=0; i<200; i++) {

							newmem(charList, size);

							auto now = osv::clock::uptime::now();
							std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
							pfleaking.insert(TStrStrPair("thread leaking malloc =>"+tmpnow,tmpnow));

							//	 std::cout << 1 << std::endl;
						}

						for (int i = 0; i < 20;i++) {
							usleep(microseconds);
							charList.show();

							auto now = osv::clock::uptime::now();
							std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());

							pfleaking.insert(TStrStrPair("thread leaking show =>"+tmpnow,tmpnow));
						}

					});




	/*std::thread t3([&] {
				// Thread with priority 1
		sched::set_adj_set();

					for ( int i=0; i<50; i++) {
						auto now = osv::clock::uptime::now();
						std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
						sched::cpu::current()->profile_overall.insert(TStrStrPair("misc t3 =>"+tmpnow,tmpnow));

						//	 std::cout << 1 << std::endl;
						for ( int j=0; j<3000000; j++) {


							// To force gcc to not optimize this loop away
							asm volatile("" : : : "memory");
						}
					}

				});


	std::thread tn([&] {
		// Thread with priority "nice"
		sched::set_adj_set();

			for ( int i=0; i<50; i++) {
				auto now = osv::clock::uptime::now();
				std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
				sched::cpu::current()->profile_overall.insert(TStrStrPair("misc tn =>"+tmpnow,tmpnow));

				//	std::cout << 2 <<  std::endl;;
				for ( int j=0; j<3000000; j++) {

					// To force gcc to not optimize this loop away
					asm volatile("" : : : "memory");
				}
			}

		});*/
	//sleep(10);
	t1.join();
	t2.join();
	/*t3.join();
	tn.join();*/
	leaking.join();

//	sched::unset_adj_set();

	pft1.show();

	pft2.show();

	pfleaking.show();

	sched::cpu::current()->profile_running_del.show();

	sched::cpu::current()->profile_4.show();

	sched::cpu::current()->profile_2.show();
	return 0;
}

