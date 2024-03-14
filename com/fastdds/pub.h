/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#ifndef _DAS_FASTDDS_PUB_H_
#define _DAS_FASTDDS_PUB_H_

#include <memory>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>

#include "../com_intf.h"



namespace banma {
namespace das {
namespace com {
namespace fast_dds {

    using namespace eprosima::fastdds::dds;

    /**
     * @brief MessageT represents the structure defined by the user in the IDL file.
    */
    template <class MessageT>
    class FastDDSPubImpl : public IPublisher<MessageT> {
    public:
        // set *large_data* to true for transmitting raw video frames, point clouds, images, etc.
        explicit FastDDSPubImpl(const std::string& topic_str, TypeSupport&& data_type, const uint32_t& domain_id = 0, const bool& reliable = false, const bool& large_data = false);
        FastDDSPubImpl(const FastDDSPubImpl& other) = default;
        FastDDSPubImpl(FastDDSPubImpl&& other) = delete;
        FastDDSPubImpl& operator=(const FastDDSPubImpl& other) = default;
        FastDDSPubImpl& operator=(FastDDSPubImpl&& other) = delete;
        virtual ~FastDDSPubImpl();

        virtual bool Init(const std::string& cfg_file_path = "") override;
        virtual COM_ERRCODE Publish(MessageT& msg) override;

    private:
        DomainParticipant* participant_;
        Publisher* publisher_;
        Topic* topic_;
        DataWriter* writer_;
        TypeSupport data_type_;
        bool wait_listener_;
        bool reliable_;
        bool large_data_;
        uint32_t no_listener_print_freq_;
        uint32_t no_listener_print_cnt_;


        class PubListener : public eprosima::fastdds::dds::DataWriterListener {
        public:
            PubListener();
            ~PubListener() override;
            bool GetFirstConnected() const;
            int GetMatched() const;
            void on_publication_matched(DataWriter* writer, const PublicationMatchedStatus& info) override;
        private:
            int n_matched_;
            bool first_connected_;
        };

        PubListener listener_;
    };


} // namespace fast_dds
} // namespace com
} // namespace das
} // namespace banma



#endif //_DAS_FASTDDS_PUB_H_