/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#include <iostream>
#include <fstream>
#include <thread>
#include "pub.h"
#include "glog/logging.h"


namespace banma {
namespace das {
namespace com {
namespace cyclone_dds {

    template <class MessageT>
    CycloneDDSPubImpl<MessageT>::CycloneDDSPubImpl(const std::string& topic, const uint32_t& domain_id, const bool& reliable, \
        bool wait_listener, const bool& large_data)

        : IPublisher<MessageT>(topic, domain_id)
        , data_writer_(MakeDataWriter(topic, domain_id, reliable))
        , wait_listener_(wait_listener)
        , large_data_(large_data)
        , no_listener_print_freq_(static_cast<uint32_t>(100))
        , no_listener_print_cnt_(static_cast<uint32_t>(0))
    {
        DLOG(INFO) << "CycloneDDSPubImpl Constructor...";
        DLOG(INFO) << "topic: " << this->GetTopicName() << ", domain_id: " << this->domain_id_;
    }

    template <class MessageT>
    CycloneDDSPubImpl<MessageT>::~CycloneDDSPubImpl()
    {
        DLOG(INFO) << "CycloneDDSPubImpl Destructor...";
    }

    template <class MessageT>
    ::dds::pub::DataWriter<MessageT> CycloneDDSPubImpl<MessageT>::MakeDataWriter(const std::string& topic_str, const uint32_t& domain_id, const bool& reliable)
    {
        ::dds::domain::DomainParticipant participant(domain_id);
        ::dds::topic::qos::TopicQos topic_qos = participant.default_topic_qos();
        if (reliable) {
            topic_qos << ::dds::core::policy::Reliability::Reliable();
        }
        ::dds::topic::Topic<MessageT> topic(participant, topic_str, topic_qos);
        ::dds::pub::Publisher publisher(participant);
        return ::dds::pub::DataWriter<MessageT>(publisher, topic);
    }

    template <class MessageT>
    bool CycloneDDSPubImpl<MessageT>::Init(const std::string& cfg_file_path)
    {
        (void)cfg_file_path;
        return true;
    }

    template <class MessageT>
    COM_ERRCODE CycloneDDSPubImpl<MessageT>::Publish(MessageT& msg)
    {
        if (this->wait_listener_) {
            const ::dds::core::status::PublicationMatchedStatus match_status = this->data_writer_.publication_matched_status();
            if (match_status.current_count() == 0) {
                // if no listener, no wait, prompt warn log
                if ((this->no_listener_print_cnt_ % this->no_listener_print_freq_) == 0) {
                    LOG(WARNING) << "No listener, skip publish msg, do nothing. wait listener enabled.";
                }
                this->no_listener_print_cnt_++;
                return COM_PUB_NO_LISTENER;
            }
        }
        this->data_writer_.write(msg);
        return COM_OK;
    }


} // namespace cyclone_dds
} // namespace com
} // namespace das
} // namespace banma