/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/
#include <chrono>
#include "sub.h"
#include "glog/logging.h"


namespace banma {
namespace das {
namespace com {
namespace cyclone_dds {

    template <class MessageT>
    CycloneDDSSubImpl<MessageT>::CycloneDDSSubImpl(const std::string& topic, const uint32_t& domain_id, const bool& reliable, \
        ::dds::sub::DataReaderListener<MessageT> *listener, const bool& large_data)
        
        : ISubscriber<MessageT>(topic, domain_id)
        , data_reader_(MakeDataReader(topic, domain_id, reliable, listener))
        , large_data_(large_data)
    {
        // DLOG(INFO) << "CycloneDDSSubImpl Constructor...";
        DLOG(INFO) << "topic: " << this->GetTopicName() << ", domain_id: " << this->domain_id_;
    }

    template <class MessageT>
    CycloneDDSSubImpl<MessageT>::~CycloneDDSSubImpl()
    {
        // DLOG(INFO) << "CycloneDDSSubImpl Destructor...";
    }

    template <class MessageT>
    ::dds::sub::DataReader<MessageT> CycloneDDSSubImpl<MessageT>::MakeDataReader(const std::string& topic_str, const uint32_t& domain_id, const bool& reliable, ::dds::sub::DataReaderListener<MessageT> *listener)
    {
        ::dds::domain::DomainParticipant participant(domain_id);
        ::dds::topic::qos::TopicQos topic_qos = participant.default_topic_qos();
        if (reliable) {
            topic_qos << ::dds::core::policy::Reliability::Reliable();
        }
        ::dds::topic::Topic<MessageT> topic(participant, topic_str, topic_qos);
        ::dds::sub::Subscriber subscriber(participant);
        
        return ::dds::sub::DataReader<MessageT>(subscriber, topic, ::dds::sub::qos::DataReaderQos(), listener, \
            listener != nullptr ? ::dds::core::status::StatusMask::data_available() : ::dds::core::status::StatusMask::none());
    }

    template <class MessageT>
    bool CycloneDDSSubImpl<MessageT>::Init(const std::string& cfg_file_path)
    {
        (void)cfg_file_path;
        return true;
    }

    template <class MessageT>
    COM_ERRCODE CycloneDDSSubImpl<MessageT>::Subscribe(std::unique_ptr<MessageT>& msg)
    {
        if ((this->samples_.length() == 0) || (this->sample_iter_ >= this->samples_.end())) {
            const ::dds::core::status::SubscriptionMatchedStatus match_status = this->data_reader_.subscription_matched_status();
            if (match_status.current_count() == 0) {
                return COM_SUB_NO_PUBLISHER;
            }
            this->samples_ = this->data_reader_.take();
            if (this->samples_.length() == 0) {
                return COM_SUB_FAILED;
            }
            this->sample_iter_ = this->samples_.begin();
        }
        while (!this->sample_iter_->info().valid()) {
            ++(this->sample_iter_);
            if (this->sample_iter_ >= this->samples_.end()) {
                return COM_SUB_FAILED;
            }
        }
        const MessageT& data = this->sample_iter_->data();
        msg.reset(new MessageT(data));
        ++(this->sample_iter_);
        return COM_OK;
    }


} // namespace cyclone_dds
} // namespace com
} // namespace das
} // namespace banma