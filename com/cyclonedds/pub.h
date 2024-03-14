/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#ifndef _DAS_CYCLONEDDS_PUB_H_
#define _DAS_CYCLONEDDS_PUB_H_

#include <memory>
#include "dds/dds.hpp"
#include "../com_intf.h"



namespace banma {
namespace das {
namespace com {
namespace cyclone_dds {

    using namespace ::org::eclipse::cyclonedds;
    /**
     * @brief MessageT represents the structure defined by the user in the IDL file.
    */
    template <class MessageT>
    class CycloneDDSPubImpl : public IPublisher<MessageT> {
    public:
        explicit CycloneDDSPubImpl(const std::string& topic, const uint32_t& domain_id = 0, const bool& reliable = false, \
            bool wait_listener = false, const bool& large_data = true);
        CycloneDDSPubImpl(const CycloneDDSPubImpl& other) = default;
        CycloneDDSPubImpl(CycloneDDSPubImpl&& other) = default;
        CycloneDDSPubImpl& operator=(const CycloneDDSPubImpl& other) = default;
        CycloneDDSPubImpl& operator=(CycloneDDSPubImpl&& other) {if (this != &other) {*this = other; return *this;}};
        virtual ~CycloneDDSPubImpl();

        virtual bool Init(const std::string& cfg_file_path = "") override;
        virtual COM_ERRCODE Publish(MessageT& msg) override;

    private:
        ::dds::pub::DataWriter<MessageT> data_writer_;
        bool wait_listener_;
        bool large_data_;
        uint32_t no_listener_print_freq_;
        uint32_t no_listener_print_cnt_;
    private:
        ::dds::pub::DataWriter<MessageT> MakeDataWriter(const std::string& topic_str, const uint32_t& domain_id, const bool& reliable);
    };


} // namespace cyclone_dds
} // namespace com
} // namespace das
} // namespace banma



#endif //_BANMA_CYCLONEDDS_PUB_H_