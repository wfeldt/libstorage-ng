

#include "storage/Storage.h"
#include "storage/StorageImpl.h"


namespace storage
{
    using std::vector;
    using std::list;


    Storage::Storage(const Environment& environment)
	: impl(new Impl(*this, environment))
    {
    }


    Storage::~Storage()
    {
    }


    const Environment&
    Storage::get_environment() const
    {
	return get_impl().get_environment();
    }


    const Arch&
    Storage::get_arch() const
    {
	return get_impl().get_arch();
    }


    Devicegraph*
    Storage::get_devicegraph(const string& name)
    {
	return get_impl().get_devicegraph(name);
    }


    const Devicegraph*
    Storage::get_devicegraph(const string& name) const
    {
	return get_impl().get_devicegraph(name);
    }


    Devicegraph*
    Storage::get_staging()
    {
	return get_impl().get_staging();
    }


    const Devicegraph*
    Storage::get_staging() const
    {
	return get_impl().get_staging();
    }


    const Devicegraph*
    Storage::get_probed() const
    {
	return get_impl().get_probed();
    }


    vector<string>
    Storage::get_devicegraph_names() const
    {
	return get_impl().get_devicegraph_names();
    }

    Devicegraph*
    Storage::create_devicegraph(const string& name)
    {
	return get_impl().create_devicegraph(name);
    }


    Devicegraph*
    Storage::copy_devicegraph(const string& source_name, const string& dest_name)
    {
	return get_impl().copy_devicegraph(source_name, dest_name);
    }


    void
    Storage::remove_devicegraph(const string& name)
    {
	get_impl().remove_devicegraph(name);
    }


    void
    Storage::restore_devicegraph(const string& name)
    {
	get_impl().restore_devicegraph(name);
    }


    bool
    Storage::exist_devicegraph(const string& name) const
    {
	return get_impl().exist_devicegraph(name);
    }


    bool
    Storage::equal_devicegraph(const string& lhs, const string& rhs) const
    {
	return get_impl().equal_devicegraph(lhs, rhs);
    }


    void
    Storage::check() const
    {
	get_impl().check();
    }


    const string&
    Storage::get_rootprefix() const
    {
	return get_impl().get_rootprefix();
    }


    void
    Storage::set_rootprefix(const string& rootprefix)
    {
	get_impl().set_rootprefix(rootprefix);
    }


    const Actiongraph*
    Storage::calculate_actiongraph()
    {
	return get_impl().calculate_actiongraph();
    }


    void
    Storage::commit(const CommitCallbacks* commit_callbacks)
    {
	get_impl().commit(commit_callbacks);
    }

}
