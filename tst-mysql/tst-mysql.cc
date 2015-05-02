/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>


/*
	This is the first attempt of MySQl example using OSv.

	1. Needs to figure out where to put the libs an bins in OSv. It turns out to be fairly easy just upload them to the same address as standard Linux does. Good compatibility there. Adding following lines to usr.manifest.skel

	/usr/include/**: /home/abc/Desktop/include/**
	/usr/lib/**: /home/abc/Desktop/lib/**

	2. Needs to figure out how to compile test application to either PIE format or as shared-object which is required by OSv. As I have no knowledge how to do it, and the tutorial available is not very easy to understand for total outsider, compile was done outside OSv with " g++ tst-mysql.cc -lmysqlcppconn -o tst-mysql.so  -fPIC -shared". Run it directly will result in"Segmentation fault (core dumped)" don't why, guess it is in PIE format. Notice that"-lmysqlcppconn" is what I don't know where to put. 

	3. Upload test applications by using adding the following line to  usr.manifest.skel

	/tst-mysql/**: /home/abc/cascading/tst-mysql/** 
	
	4 Launching MySQL with following script:
sudo qemu-system-x86_64 -m 2G -smp 4 -device virtio-blk-pci,id=blk0,bootindex=0,drive=hd0,scsi=off -drive file=/home/abc/tomosv/build/release/usr.img,if=none,id=hd0,aio=native,cache=none -netdev tap,id=hn0,script=scripts/qemu-ifup.sh,vhost=on -device virtio-net-pci,netdev=hn0,id=nic0,mac=DE:AD:BE:EF:76:0D  -device virtio-rng-pci -enable-kvm -cpu host,+x2apic -chardev stdio,mux=on,id=stdio,signal=off -mon chardev=stdio,mode=readline,default -device isa-serial,chardev=stdio

(IP address will be 192.168.122.75 )

	5 Launching test application is a bit of trick since there is no where to configure the command line to execute the external application. The run.py shows the command line is sort of hardcoded to the image file but not as qemu parameters. It makes no sense specifying an application to run during launching. A easy fix is to use standard launching script ./script/run.py - e "tst-mysql.so" to hardcode external application into the command line after make. This way, the command line will be there until a new command line is specified. After, launching test case use following script:
	sudo qemu-system-x86_64  -m 2G -smp 4 -device virtio-blk-pci,id=blk0,bootindex=0,drive=hd0,scsi=off -drive file=/home/abc/osv7/build/release/usr.img,if=none,id=hd0,aio=native,cache=none -netdev tap,id=hn0,script=scripts/qemu-ifup.sh,vhost=on -device virtio-net-pci,netdev=hn0,id=nic0,mac=DE:AD:BE:EF:76:0E -device virtio-rng-pci -enable-kvm -cpu host,+x2apic -chardev stdio,mux=on,id=stdio,signal=off -mon chardev=stdio,mode=readline,default -device isa-serial,chardev=stdio

(IP address will be 192.168.122.76 )

 
Code is totally borrowed.


*/


/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

int main(void)
{
cout << endl;
cout << "Running 'SELECT 'Hello World!' AS _message'..." << endl;

try {
  sql::Driver *driver;
  sql::Connection *con;
  sql::Statement *stmt;
  sql::ResultSet *res;

  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://192.168.122.75:3306", "admin", "osv");
  /* Connect to the MySQL test database */
  con->setSchema("test");

  stmt = con->createStatement();
  res = stmt->executeQuery("SELECT 'Hello World!' AS _message"); // replace with your statement
  while (res->next()) {
    cout << "\t... MySQL replies: ";
    /* Access column data by alias or column name */
    cout << res->getString("_message") << endl;
    cout << "\t... MySQL says it again: ";
    /* Access column fata by numeric offset, 1 is the first column */
    cout << res->getString(1) << endl;
  }
  delete res;
  delete stmt;
  delete con;

} catch (sql::SQLException &e) {
  cout << "# ERR: SQLException in " << __FILE__;
  cout << "(" << __FUNCTION__ << ") on line "  << __LINE__ << endl;
  cout << "# ERR: " << e.what();
  cout << " (MySQL error code: " << e.getErrorCode();
  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}

cout << endl;

return EXIT_SUCCESS;
}

