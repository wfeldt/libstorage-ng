

#include <iostream>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


void
partitions()
{
    Devicegraph devicegraph;

    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Gpt* gpt = Gpt::create(&devicegraph);
    User::create(&devicegraph, sda, gpt);

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1", Region(0, 1000, 262144), PartitionType::PRIMARY);
    Subdevice::create(&devicegraph, gpt, sda1);

    gpt->create_partition("/dev/sda2", Region(1000, 1000, 262144), PartitionType::PRIMARY);

    devicegraph.check();
    cout << devicegraph << endl;

    for (const Partition* partition : gpt->get_partitions())
    {
	cout << partition->get_name() << endl;
    }
}


int
main()
{
    partitions();
}
