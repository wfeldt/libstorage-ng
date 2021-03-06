

#include "storage/Devices/SwapImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Swap*
    Swap::create(Devicegraph* devicegraph)
    {
	Swap* ret = new Swap(new Swap::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Swap*
    Swap::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Swap* ret = new Swap(new Swap::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Swap::Swap(Impl* impl)
	: Filesystem(impl)
    {
    }


    Swap*
    Swap::clone() const
    {
	return new Swap(get_impl().clone());
    }


    Swap::Impl&
    Swap::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Swap::Impl&
    Swap::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_swap(const Device* device)
    {
	return is_device_of_type<const Swap>(device);
    }


    Swap*
    to_swap(Device* device)
    {
	return to_device_of_type<Swap>(device);
    }


    const Swap*
    to_swap(const Device* device)
    {
	return to_device_of_type<const Swap>(device);
    }

}
