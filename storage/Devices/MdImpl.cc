

#include <ctype.h>
#include <iostream>
#include <boost/regex.hpp>

#include "storage/Devices/MdImpl.h"
#include "storage/Holders/MdUser.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Md>::classname = "Md";


    // strings must match /proc/mdstat
    const vector<string> EnumTraits<MdLevel>::names({
	"unknown", "RAID0", "RAID1", "RAID5", "RAID6", "RAID10"
    });


    // strings must match "mdadm --parity" option
    const vector<string> EnumTraits<MdParity>::names({
	"default", "left-asymmetric", "left-symmetric", "right-asymmetric",
	"right-symmetric", "parity-first", "parity-last", "left-asymmetric-6",
	"left-symmetric-6", "right-asymmetric-6", "right-symmetric-6",
	"parity-first-6", "n2", "o2", "f2", "n3", "o3", "f3"
    });


    Md::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), md_level(RAID0), md_parity(DEFAULT), chunk_size_k(0)
    {
	string tmp;

	if (getChildValue(node, "md-level", tmp))
	    md_level = toValueWithFallback(tmp, RAID0);

	if (getChildValue(node, "md-parity", tmp))
	    md_parity = toValueWithFallback(tmp, DEFAULT);

	getChildValue(node, "chunk-size-k", chunk_size_k);
    }


    void
    Md::Impl::set_md_level(MdLevel md_level)
    {
	// TODO calculate size_k

	Impl::md_level = md_level;
    }


    void
    Md::Impl::set_chunk_size_k(unsigned long chunk_size_k)
    {
	// TODO calculate size_k

	Impl::chunk_size_k = chunk_size_k;
    }


    bool
    Md::Impl::is_valid_name(const string& name)
    {
	static boost::regex name_regex(DEVDIR "/md[0-9]+", boost::regex_constants::extended);

	return boost::regex_match(name, name_regex);
    }


    vector<string>
    Md::Impl::probe_mds(SystemInfo& systeminfo)
    {
	vector<string> ret;

	for (const string& short_name : systeminfo.getDir(SYSFSDIR "/block"))
	{
	    string name = DEVDIR "/" + short_name;

	    if (!is_valid_name(name))
		continue;

	    ret.push_back(name);
	}

	return ret;
    }


    void
    Md::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Partitionable::Impl::probe_pass_1(probed, systeminfo);

	string tmp = get_name().substr(strlen(DEVDIR "/"));

	ProcMdstat::Entry entry;
	if (!systeminfo.getProcMdstat().getEntry(tmp, entry))
	{
	    // TODO
	    throw;
	}

	md_level = entry.md_level;
	md_parity = entry.md_parity;

	chunk_size_k = entry.chunk_size_k;
    }


    void
    Md::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	string tmp = get_name().substr(strlen(DEVDIR "/"));

	ProcMdstat::Entry entry;
	if (!systeminfo.getProcMdstat().getEntry(tmp, entry))
	{
	    // TODO
	    throw;
	}

	for (const ProcMdstat::Device& device : entry.devices)
	{
	    BlkDevice* blk_device = BlkDevice::find(probed, device.name);
	    MdUser* md_user = MdUser::create(probed, blk_device, get_device());
	    md_user->set_spare(device.spare);
	    md_user->set_faulty(device.faulty);
	}
    }


    void
    Md::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::AddEtcMdadm(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Md::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::RemoveEtcMdadm(get_sid()));
	actions.push_back(new Action::Delete(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Md::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "md-level", toString(md_level));
	setChildValueIf(node, "md-parity", toString(md_parity), md_parity != DEFAULT);

	setChildValueIf(node, "chunk-size-k", chunk_size_k, chunk_size_k != 0);
    }


    MdUser*
    Md::Impl::add_device(BlkDevice* blk_device)
    {
	if (blk_device->num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(blk_device->num_children(), 0));

	// TODO calculate size_k

	// TODO set partition id?

	return MdUser::create(get_devicegraph(), blk_device, get_device());
    }


    void
    Md::Impl::remove_device(BlkDevice* blk_device)
    {
	MdUser* md_user = to_md_user(get_devicegraph()->find_holder(blk_device->get_sid(), get_sid()));

	get_devicegraph()->remove_holder(md_user);

	// TODO calculate size_k
    }


    vector<BlkDevice*>
    Md::Impl::get_devices()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO sorting

	return devicegraph.filter_devices_of_type<BlkDevice>(devicegraph.parents(vertex));
    }


    vector<const BlkDevice*>
    Md::Impl::get_devices() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO sorting

	return devicegraph.filter_devices_of_type<const BlkDevice>(devicegraph.parents(vertex));
    }


    unsigned int
    Md::Impl::get_number() const
    {
	string::size_type pos = get_name().find_last_not_of("0123456789");
	if (pos == string::npos || pos == get_name().size() - 1)
	    ST_THROW(Exception("md name has no number"));

	return atoi(get_name().substr(pos + 1).c_str());
    }


    bool
    Md::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return md_level == rhs.md_level && md_parity == rhs.md_parity &&
	    chunk_size_k == rhs.chunk_size_k;
    }


    void
    Md::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "md-level", md_level, rhs.md_level);
	storage::log_diff_enum(log, "md-parity", md_parity, rhs.md_parity);

	storage::log_diff(log, "chunk-size-k", chunk_size_k, rhs.chunk_size_k);
    }


    void
    Md::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " md-level:" << toString(get_md_level());
	out << " md-parity:" << toString(get_md_parity());

	out << " chunk-size-k:" << get_chunk_size_k();
    }


    void
    Md::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	partition(udev_ids.begin(), udev_ids.end(), string_starts_with("md-uuid-"));
    }


    Text
    Md::Impl::do_create_text(Tense tense) const
    {
	return sformat(_("Create MD RAID %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }


    void
    Md::Impl::do_create() const
    {
	string cmd_line = MDADMBIN " --create " + quote(get_name()) + " --run --level=" +
	    boost::to_lower_copy(toString(md_level), locale::classic()) + " -e 1.0 --homehost=any";

	if (md_level == RAID1 || md_level == RAID5 || md_level == RAID6 || md_level == RAID10)
	    cmd_line += " -b internal";

	if (chunk_size_k > 0)
	    cmd_line += " --chunk=" + to_string(chunk_size_k);

	if (md_parity != DEFAULT)
	    cmd_line += " --parity=" + toString(md_parity);

	vector<string> devices;
	vector<string> spares;

	for (const BlkDevice* blk_device : get_devices())
	{
	    bool spare = false;

	    // TODO add get_out_holder that throws if num_children != 1, like get_single_child_of_type

	    for (const Holder* out_holder : blk_device->get_out_holders())
	    {
		if (to_md_user(out_holder)->is_spare())
		{
		    spare = true;
		    break;
		}
	    }

	    if (!spare)
		devices.push_back(blk_device->get_name());
	    else
		spares.push_back(blk_device->get_name());
	}

	cmd_line += " --raid-devices=" + to_string(devices.size());

	if (!spares.empty())
	    cmd_line += " --spare-devices=" + to_string(spares.size());

	cmd_line += " " + quote(devices) + " " + quote(spares);

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create md raid failed"));
    }


    Text
    Md::Impl::do_delete_text(Tense tense) const
    {
	return sformat(_("Delete MD RAID %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }


    void
    Md::Impl::do_delete() const
    {
	// TODO split into deactivate and delete?

	string cmd_line = MDADMBIN " --stop " + quote(get_name());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete md raid failed"));

	for (const BlkDevice* blk_device : get_devices())
	{
	    blk_device->get_impl().wipe_device();
	}
    }


    Text
    Md::Impl::do_add_etc_mdadm_text(Tense tense) const
    {
	return sformat(_("Add %1$s to /etc/mdadm.conf"), get_name().c_str());
    }


    void
    Md::Impl::do_add_etc_mdadm(const Actiongraph::Impl& actiongraph) const
    {
	// TODO
    }


    Text
    Md::Impl::do_remove_etc_mdadm_text(Tense tense) const
    {
	return sformat(_("Remove %1$s from /etc/mdadm.conf"), get_name().c_str());
    }


    void
    Md::Impl::do_remove_etc_mdadm(const Actiongraph::Impl& actiongraph) const
    {
	// TODO
    }


    Text
    Md::Impl::do_reallot_text(ReallotMode reallot_mode, const BlkDevice* blk_device, Tense tense) const
    {
	Text text;

	switch (reallot_mode)
	{
	    case ReallotMode::REDUCE:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Remove %1$s from %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Removing %1$s from %2$s"));
		break;

	    case ReallotMode::EXTEND:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Add %1$s to %2$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by device name (e.g. /dev/sdd),
			      // %2$s is replaced by device name (e.g. /dev/md0)
			      _("Adding %1$s to %2$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for reallot_mode"));
	}

	return sformat(text, blk_device->get_name().c_str(), get_displayname().c_str());
    }


    void
    Md::Impl::do_reallot(ReallotMode reallot_mode, const BlkDevice* blk_device) const
    {
	switch (reallot_mode)
	{
	    case ReallotMode::REDUCE:
		do_reduce(blk_device);
		return;

	    case ReallotMode::EXTEND:
		do_extend(blk_device);
		return;
	}

	ST_THROW(LogicException("invalid value for reallot_mode"));
    }


    void
    Md::Impl::do_reduce(const BlkDevice* blk_device) const
    {
	string cmd_line = MDADMBIN " --remove " + quote(get_name()) + " " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("reduce md failed"));

	// Thanks to udev "md-raid-assembly.rules" running "parted <disk>
	// print" readds the device to the md if the signature is still
	// valid. Thus remove the signature.
	blk_device->get_impl().wipe_device();
    }


    void
    Md::Impl::do_extend(const BlkDevice* blk_device) const
    {
	string cmd_line = MDADMBIN " --add " + quote(get_name()) + " " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("extend md failed"));
    }


    namespace Action
    {

	Text
	AddEtcMdadm::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Md* md = to_md(get_device_rhs(actiongraph));
	    return md->get_impl().do_add_etc_mdadm_text(tense);
	}


	void
	AddEtcMdadm::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Md* md = to_md(get_device_rhs(actiongraph));
	    md->get_impl().do_add_etc_mdadm(actiongraph);
	}


	void
	AddEtcMdadm::add_dependencies(Actiongraph::Impl::vertex_descriptor v,
				      Actiongraph::Impl& actiongraph) const
	{
	    if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		actiongraph.add_edge(*actiongraph.mount_root_filesystem, v);
	}


	Text
	RemoveEtcMdadm::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Md* md = to_md(get_device_lhs(actiongraph));
	    return md->get_impl().do_remove_etc_mdadm_text(tense);
	}


	void
	RemoveEtcMdadm::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Md* md = to_md(get_device_lhs(actiongraph));
	    md->get_impl().do_remove_etc_mdadm(actiongraph);
	}

    }


    bool
    compare_by_number(const Md* lhs, const Md* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }

}
