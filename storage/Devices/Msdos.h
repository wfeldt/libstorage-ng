#ifndef STORAGE_MSDOS_H
#define STORAGE_MSDOS_H


#include "storage/Devices/PartitionTable.h"


namespace storage
{

    class Msdos : public PartitionTable
    {
    public:

	static Msdos* create(Devicegraph* devicegraph);
	static Msdos* load(Devicegraph* devicegraph, const xmlNode* node);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Msdos* clone() const override;

    protected:

	Msdos(Impl* impl);

    };


    bool is_msdos(const Device* device);

    Msdos* to_msdos(Device* device);
    const Msdos* to_msdos(const Device* device);

}


#endif
