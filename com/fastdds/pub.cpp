/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#include <iostream>
#include <fstream>
#include <mutex>
#include "pub.h"
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include "glog/logging.h"


namespace banma {
namespace das {
namespace com {
namespace fast_dds {

    template <class MessageT>
    FastDDSPubImpl<MessageT>::FastDDSPubImpl(const std::string& topic_str, TypeSupport&& data_type, const uint32_t& domain_id, const bool& reliable, const bool& large_data)
        : IPublisher<MessageT>(topic_str, domain_id)
        , participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , writer_(nullptr)
        , data_type_(data_type)
        , wait_listener_(false)
        , reliable_(reliable)
        , large_data_(large_data)
        , no_listener_print_freq_(static_cast<uint32_t>(100))
        , no_listener_print_cnt_(static_cast<uint32_t>(0))
    {
        DLOG(INFO) << "FastDDSPubImpl Constructor...";
    }

    template <class MessageT>
    FastDDSPubImpl<MessageT>::~FastDDSPubImpl()
    {
        DLOG(INFO) << "FastDDSPubImpl Destructor...";
        if (writer_ != nullptr) {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr) {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr) {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    template <class MessageT>
    bool FastDDSPubImpl<MessageT>::Init(const std::string& cfg_file_path)
    {
        (void)cfg_file_path;
        DomainParticipantQos pqos = PARTICIPANT_QOS_DEFAULT;
        if (this->large_data_) {
            /* Transports configuration */
            #if 1
            // UDPv4 transport for PDP over multicast and SHM / TCPv4 transport for EDP and application data
            pqos.setup_transports(eprosima::fastdds::rtps::BuiltinTransports::LARGE_DATA);
            #endif
            // Increase the sending buffer size, besides, sudo sysctl -w net.core.wmem_max=2000000
            pqos.transport().send_socket_buffer_size = 2000000;
            // Increase the receiving buffer size, besides, sudo sysctl -w net.core.rmem_max=2000000
            pqos.transport().listen_socket_buffer_size = 2000000;
        }
        
        this->participant_ = DomainParticipantFactory::get_instance()->create_participant(this->domain_id_, pqos);
        if (this->participant_ == nullptr) {
            return false;
        }

        this->data_type_.register_type(this->participant_);

        this->publisher_ = this->participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
        if (publisher_ == nullptr) {
            return false;
        }

        this->topic_ = this->participant_->create_topic(this->GetTopicName(), this->data_type_.get_type_name(), TOPIC_QOS_DEFAULT);
        if (this->topic_ == nullptr) {
            return false;
        }

        DataWriterQos wqos = DATAWRITER_QOS_DEFAULT;
        wqos.data_sharing().automatic();
        if (this->reliable_) {
            wqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
        }
        // large data transmit optimize
        if (this->large_data_ && wqos.reliability().kind == RELIABLE_RELIABILITY_QOS) {
            LOG(INFO) << "====LARGE_DATA & RELIABLE_RELIABILITY_QOS optimize...";
            wqos.reliable_writer_qos().times.heartbeatPeriod.seconds = 0;
            wqos.reliable_writer_qos().times.heartbeatPeriod.nanosec = 500000000; //500 ms
            // Using Non-strict Reliability
            wqos.history().kind =  KEEP_LAST_HISTORY_QOS;
            wqos.history().depth = 20;
        }
        writer_ = this->publisher_->create_datawriter(this->topic_, wqos, &this->listener_);
        if (writer_ == nullptr) {
            return false;
        }
        return true;
    }

    template <class MessageT>
    COM_ERRCODE FastDDSPubImpl<MessageT>::Publish(MessageT& msg)
    {
        if (this->wait_listener_ && this->listener_.GetMatched() <= 0) {
            if ((this->no_listener_print_cnt_ % this->no_listener_print_freq_) == 0) {
                LOG(WARNING) << "No listener, skip publish msg.";
            }
            this->no_listener_print_cnt_++;
            return COM_PUB_NO_LISTENER;
        }
        if (!this->writer_->write(reinterpret_cast<void*>(&msg))) {
            LOG(ERROR) << "msg write failed!";
            return COM_PUB_FAILED;
        }
        return COM_OK;
    }

    //==========================================================================

    template <class MessageT>
    FastDDSPubImpl<MessageT>::PubListener::PubListener()
        : n_matched_(0)
        , first_connected_(false)
    {

    }
    template <class MessageT>
    FastDDSPubImpl<MessageT>::PubListener::~PubListener()
    {

    }
    template <class MessageT>
    void FastDDSPubImpl<MessageT>::PubListener::on_publication_matched(DataWriter* writer, const PublicationMatchedStatus& info)
    {
        (void)writer;
        if (info.current_count_change == 1) {
            this->n_matched_ = info.total_count;
            this->first_connected_ = true;
            std::cout << "Publisher matched. n_matched_: " << this->n_matched_ << std::endl;
        }
        else if (info.current_count_change == -1) {
            this->n_matched_ = info.total_count;
            std::cout << "Publisher unmatched. n_matched_: " << this->n_matched_ << std::endl;
        }
        else {
            std::cout << info.current_count_change
                    << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
        }
    }

    template <class MessageT>
    int FastDDSPubImpl<MessageT>::PubListener::GetMatched() const {
        return this->n_matched_;
    }

    template <class MessageT>
    bool FastDDSPubImpl<MessageT>::PubListener::GetFirstConnected() const {
        return this->first_connected_;
    }

} // namespace fast_dds
} // namespace com
} // namespace das
} // namespace banma