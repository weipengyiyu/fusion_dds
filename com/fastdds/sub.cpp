/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#include <iostream>
#include <fstream>
#include "sub.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include "glog/logging.h"


namespace banma {
namespace das {
namespace com {
namespace fast_dds {

    template <class MessageT>
    SubListener<MessageT>::SubListener(const uint32_t& msg_q_cap)
        : info_()
        , n_matched_(0)
        , n_samples_(static_cast<uint32_t>(0))
        , msg_queue_()
        , msg_q_cap_(msg_q_cap)
        , msg_q_mtx_()
    {

    }

    template <class MessageT>
    SubListener<MessageT>::~SubListener()
    {

    }

    template <class MessageT>
    void SubListener<MessageT>::on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info)
    {
        (void)reader;
        if (info.current_count_change == 1) {
            this->n_matched_ = info.total_count;
            LOG(INFO) << "Subscriber matched. matched_: " << this->n_matched_ << std::endl;
        }
        else if (info.current_count_change == -1) {
            this->n_matched_ = info.total_count;
            LOG(INFO) << "Subscriber unmatched. matched_: " << this->n_matched_ << std::endl;
        }
        else {
            LOG(INFO) << info.current_count_change
                    << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
        }
    }

    template <class MessageT>
    void SubListener<MessageT>::on_data_available(DataReader* reader)
    {
        std::unique_ptr<MessageT> msg(new MessageT());
        
        auto ret = reader->take_next_sample(reinterpret_cast<void*>(msg.get()), &this->info_);
        if (ret == ReturnCode_t::RETCODE_OK && this->info_.instance_state == ALIVE_INSTANCE_STATE) {
            this->Push(std::move(msg));
            this->n_samples_++;
            if ((this->n_samples_ % 100) == 0) {
                DLOG(INFO) << "Message received total: "<< this->n_samples_;
            }
        }
    }

    template <class MessageT>
    std::unique_ptr<MessageT> SubListener<MessageT>::Pop()
    {
        std::unique_ptr<MessageT> msg(nullptr);
        std::lock_guard<std::mutex> lock(this->msg_q_mtx_);
        if (this->msg_queue_.empty()) {
            return msg;
        }
        msg = std::move(this->msg_queue_.front());
        this->msg_queue_.pop();
        return msg;
    }

    template <class MessageT>
    bool SubListener<MessageT>::Push(std::unique_ptr<MessageT> msg)
    {
        std::lock_guard<std::mutex> lock(this->msg_q_mtx_);
        if (static_cast<uint32_t>(this->msg_queue_.size()) >= this->msg_q_cap_) {
            LOG(WARNING) << "msg queue over limit! " << this->msg_queue_.size() << ", drop the oldest!";
            while (static_cast<uint32_t>(this->msg_queue_.size()) >= this->msg_q_cap_) {
                this->msg_queue_.pop();
            }
        }
        this->msg_queue_.push(std::move(msg));
        return true;
    }

    template <class MessageT>
    int SubListener<MessageT>::GetMatched() const
    {
        return this->n_matched_;
    }

    //==========================================================================
    template <class MessageT>
    FastDDSSubImpl<MessageT>::FastDDSSubImpl(const std::string& topic_str, TypeSupport&& data_type, const uint32_t& domain_id, const bool& reliable, const bool& large_data)
        : ISubscriber<MessageT>(topic_str, domain_id)
        , participant_(nullptr)
        , subscriber_(nullptr)
        , topic_(nullptr)
        , reader_(nullptr)
        , data_type_(data_type)
        , reliable_(reliable)
        , large_data_(large_data)
        , listener_()
    {

    }

    template <class MessageT>
    FastDDSSubImpl<MessageT>::~FastDDSSubImpl()
    {
        if (this->reader_ != nullptr) {
            this->subscriber_->delete_datareader(this->reader_);
        }
        if (this->topic_ != nullptr) {
            participant_->delete_topic(this->topic_);
        }
        if (this->subscriber_ != nullptr) {
            this->participant_->delete_subscriber(this->subscriber_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(this->participant_);
    }

    template <class MessageT>
    bool FastDDSSubImpl<MessageT>::Init(const std::string& cfg_file_path)
    {
        (void)cfg_file_path;
        DomainParticipantQos pqos;

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

        this->participant_ = DomainParticipantFactory::get_instance()->create_participant(0, pqos);
        if (this->participant_ == nullptr) {
            return false;
        }
        this->data_type_.register_type(this->participant_);
        this->subscriber_ = this->participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
        if (this->subscriber_ == nullptr) {
            return false;
        }
        this->topic_ = this->participant_->create_topic(this->GetTopicName(), this->data_type_.get_type_name(), TOPIC_QOS_DEFAULT);
        if (this->topic_ == nullptr) {
            return false;
        }
        DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
        if (this->reliable_) {
            rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
        }
        // large data transmit optimize
        if (this->large_data_ && rqos.reliability().kind == RELIABLE_RELIABILITY_QOS) {
            LOG(INFO) << "====LARGE_DATA & RELIABLE_RELIABILITY_QOS optimize...";
            // Using Non-strict Reliability
            rqos.history().kind =  KEEP_LAST_HISTORY_QOS;
            rqos.history().depth = 20;
        }
        rqos.durability().kind = VOLATILE_DURABILITY_QOS;
        rqos.data_sharing().automatic();
        this->reader_ = subscriber_->create_datareader(this->topic_, rqos, &this->listener_);
        if (this->reader_ == nullptr) {
            return false;
        }
        return true;
    }

    template <class MessageT>
    COM_ERRCODE FastDDSSubImpl<MessageT>::Subscribe(std::unique_ptr<MessageT>& out)
    {
        std::unique_ptr<MessageT> msg(nullptr);
        msg = this->listener_.Pop();
        if (msg) {
            out = std::move(msg);
            return COM_OK;
        }
        if (!this->listener_.GetMatched()) {
            return COM_SUB_NO_PUBLISHER;
        }
        return COM_SUB_FAILED;
    }


} // namespace fast_dds
} // namespace com
} // namespace das
} // namespace banma