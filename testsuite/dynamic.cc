
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dynamic)
{
    DeviceGraph device_graph;

    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, sda, sda1);

    LvmVg* system = LvmVg::create(device_graph, "/dev/system");
    Using::create(device_graph, sda1, system);

    LvmLv* root = LvmLv::create(device_graph, "/dev/system/root");
    Subdevice::create(device_graph, system, root);

    BOOST_CHECK_EQUAL(device_graph.num_vertices(), 4);
    BOOST_CHECK_EQUAL(device_graph.num_edges(), 3);

    BOOST_CHECK(dynamic_cast<const Disk*>(device_graph.find_device(sda->getSid())));

    BOOST_CHECK(dynamic_cast<const Partition*>(device_graph.find_device(sda1->getSid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(device_graph.find_holder(sda->getSid(), sda1->getSid())));

    BOOST_CHECK(dynamic_cast<const LvmVg*>(device_graph.find_device(system->getSid())));
    BOOST_CHECK(dynamic_cast<const Using*>(device_graph.find_holder(sda1->getSid(), system->getSid())));

    BOOST_CHECK(dynamic_cast<const LvmLv*>(device_graph.find_device(root->getSid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(device_graph.find_holder(system->getSid(), root->getSid())));
}
