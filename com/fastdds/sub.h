/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#ifndef _DAS_FASTDDS_SUB_H_
#define _DAS_FASTDDS_SUB_H_

#include <memory>
#include <queue>
#include <mutex>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/core/status/SubscriptionMatchedStatus.hpp>
#include "../com_intf.h"



namespace banma {
namespace das {
namespace com {
namespace fast_dds {

    using namespace eprosima::fastdds::dds;

    template <class MessageT>
    class SubListener : public DataReaderListener {
    public:
        explicit SubListener(const uint32_t& msg_q_cap = 100);
        SubListener(const SubListener& other) = default;
        SubListener(SubListener&& other) = default;
        SubListener& operator=(const SubListener& other) = default;
        SubListener& operator=(SubListener&& other) = default;
        ~SubListener();
        void on_data_available(DataReader* reader) override;
        void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override;
        std::unique_ptr<MessageT> Pop();
        bool Push(std::unique_ptr<MessageT> msg);
        int GetMatched() const;

        SampleInfo info_;
        int n_matched_;
        uint32_t n_samples_;
        std::queue<std::unique_ptr<MessageT>> msg_queue_;
        uint32_t msg_q_cap_;
        std::mutex msg_q_mtx_;
    };

    /**
     * @brief MessageT represents the structure defined by the user in the IDL file.
    */
    template <class MessageT>
    class FastDDSSubImpl : public ISubscriber<MessageT> {
    public:
        explicit FastDDSSubImpl(const std::string& topic_str, TypeSupport&& data_type, const uint32_t& domain_id = 0, const bool& reliable = false, const bool& large_data = false);
        FastDDSSubImpl(const FastDDSSubImpl& other) = default;
        FastDDSSubImpl(FastDDSSubImpl&& other) = delete;
        FastDDSSubImpl& operator=(const FastDDSSubImpl& other) = default;
        FastDDSSubImpl& operator=(FastDDSSubImpl&& other) = delete;
        virtual ~FastDDSSubImpl();

        virtual bool Init(const std::string& cfg_file_path = "") override;
        virtual COM_ERRCODE Subscribe(std::unique_ptr<MessageT>& msg) override;

    private:
        DomainParticipant* participant_;
        Subscriber* subscriber_;
        Topic* topic_;
        DataReader* reader_;
        TypeSupport data_type_;
        bool reliable_;
        bool large_data_;
        SubListener<MessageT> listener_;
    };


} // namespace fast_dds
} // namespace com
} // namespace das
} // namespace banma



#endif //_DAS_FASTDDS_SUB_H_