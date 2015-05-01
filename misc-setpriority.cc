/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

// Use this benchmark to calibrate setpriority() to have a similar effect
// to what it does in Linux. I.e., when a thread that does setpriority(10)
// competes with a thread with default priority (0), what is the runtime
// ratio they get? This benchmark can run on both OSv and Linux.
//
// In OSv's internal API, thread::setpriority() has a very well-defined
// meaning: A thread with OSv priority "x" competing against a thread
// with OSv priority 1 will get 1/x times the runtime. In the POSIX
// setpriority() API, no such definition exists, and the amount of CPU
// time that a thread with certain nice value will get varied between
// Unix variants, and Linux versions. The Linux setpriority(2) manual
// explains that starting with Linux 2.6.23, the effect of setpriority()
// has become more extreme (low-priority threads now get much less
// run time), and with this benchmark we can measure exactly how.
//
// To run this benchmark on Linux:
//    g++ -g -pthread -std=c++11 tests/misc-setpriority.cc
//    sudo taskset 1 ./a.out
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <atomic>
#include "osv/sched.hh"

#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>

typedef std::map<std::string, std::string> TStrStrMap;
typedef std::pair<std::string, std::string> TStrStrPair;

int main() {



	std::thread t1([&] {
		// Thread with priority 1
		sched::set_adj_set();

			for ( int i=0; i<50; i++) {
				auto now = osv::clock::uptime::now();
				std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
				sched::cpu::current()->profile_overall.insert(TStrStrPair("misc t1 =>"+tmpnow,tmpnow));

				//	 std::cout << 1 << std::endl;
				for ( int j=0; j<3000000; j++) {


					// To force gcc to not optimize this loop away
					asm volatile("" : : : "memory");
				}
			}

		});

	std::thread t2([&] {
			// Thread with priority 1
		sched::set_adj_set();

				for ( int i=0; i<50; i++) {
					auto now = osv::clock::uptime::now();
					std::string tmpnow = std::to_string( std::chrono::duration_cast<std::chrono::microseconds>( now.time_since_epoch()).count());
					sched::cpu::current()->profile_overall.insert(TStrStrPair("misc t2 =>"+tmpnow,tmpnow));

					//	 std::cout << 1 << std::endl;
					for ( int j=0; j<3000000; j++) {


						// To force gcc to not optimize this loop away
						asm volatile("" : : : "memory");
					}
				}

			});

	std::thread t3([&] {
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

		});
	sleep(10);
	t1.join();
	t2.join();
	t3.join();
	tn.join();


	sched::unset_adj_set();

	sched::cpu::current()->profile_running_del.show();

	sched::cpu::current()->profile_4.show();

	sched::cpu::current()->profile_overall.show();

	sched::cpu::current()->profile_2.show();
	return 0;
}

