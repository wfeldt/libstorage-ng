

#include "storage/Devices/EncryptionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Encryption>::classname = "Encryption";


    const vector<string> EnumTraits<EncryptionType>::names({
	"none", "twofish256", "twofish", "twofishSL92", "luks", "unknown"
    });


    Encryption::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    Encryption::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    void
    Encryption::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::OpenEncryption(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Encryption::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    Encryption::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);
    }


    void
    Encryption::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }


    Text
    Encryption::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create encryption on %1$s"), get_displayname().c_str());
    }


    Text
    Encryption::Impl::do_open_text(Tense tense) const
    {
	return sformat(_("Open encryption on %1$s"), get_displayname().c_str());
    }


    namespace Action
    {

	Text
	OpenEncryption::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Encryption* encryption = to_encryption(get_device_rhs(actiongraph));
	    return encryption->get_impl().do_open_text(tense);
	}

	void
	OpenEncryption::commit(const Actiongraph::Impl& actiongraph) const
	{
	    // TODO
	}

    }

}
