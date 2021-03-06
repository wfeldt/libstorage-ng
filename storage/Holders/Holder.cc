

#include "storage/Holders/HolderImpl.h"
#include "storage/Devices/DeviceImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    HolderHasWrongType::HolderHasWrongType(const char* seen, const char* expected)
	: Exception(sformat("holder has wrong type, seen '%s', expected '%s'", seen, expected),
		    Silencer::is_any_active() ? DEBUG : WARNING)
    {
    }


    Holder::Holder(Impl* impl)
	: impl(impl)
    {
	ST_CHECK_PTR(impl);
    }


    Holder::~Holder()
    {
    }


    bool
    Holder::operator==(const Holder& rhs) const
    {
	return get_impl().operator==(rhs.get_impl());
    }


    bool
    Holder::operator!=(const Holder& rhs) const
    {
	return get_impl().operator!=(rhs.get_impl());
    }


    void
    Holder::create(Devicegraph* devicegraph, const Device* source, const Device* target)
    {
	add_to_devicegraph(devicegraph, source, target);
    }


    void
    Holder::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	sid_t source_sid = 0;
	if (!getChildValue(node, "source-sid", source_sid))
	    ST_THROW(Exception("no source-sid"));

	sid_t target_sid = 0;
	if (!getChildValue(node, "target-sid", target_sid))
	    ST_THROW(Exception("no target-sid"));

	const Device* source = devicegraph->find_device(source_sid);
	const Device* target = devicegraph->find_device(target_sid);

	add_to_devicegraph(devicegraph, source, target);
    }


    sid_t
    Holder::get_source_sid() const
    {
	return get_impl().get_source_sid();
    }


    sid_t
    Holder::get_target_sid() const
    {
	return get_impl().get_target_sid();
    }


    void
    Holder::add_to_devicegraph(Devicegraph* devicegraph, const Device* source,
			       const Device* target)
    {
	ST_CHECK_PTR(devicegraph);
	ST_CHECK_PTR(source);
	ST_CHECK_PTR(target);

	if (source->get_impl().get_devicegraph() != devicegraph)
	    ST_THROW(Exception("wrong graph in source"));

	if (target->get_impl().get_devicegraph() != devicegraph)
	    ST_THROW(Exception("wrong graph in target"));

	Devicegraph::Impl::vertex_descriptor source_vertex = source->get_impl().get_vertex();
	Devicegraph::Impl::vertex_descriptor target_vertex = target->get_impl().get_vertex();

	Devicegraph::Impl::edge_descriptor edge = devicegraph->get_impl().add_edge(source_vertex, target_vertex, this);

	get_impl().set_devicegraph_and_edge(devicegraph, edge);
    }


    void
    Holder::save(xmlNode* node) const
    {
	get_impl().save(node);
    }


    std::ostream&
    operator<<(std::ostream& out, const Holder& holder)
    {
	holder.get_impl().print(out);
	return out;
    }

}
