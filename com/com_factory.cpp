/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/


#include <memory>
#include "com_factory.h"

#ifdef USE_FASTDDS
#include "fastdds/pub.h"
#include "fastdds/sub.h"
#endif // USE_FASTDDS

#ifdef USE_CYCLONEDDS
#include "cyclonedds/pub.h"
#include "cyclonedds/sub.h"
#endif // USE_CYCLONEDDS

#ifdef USE_ROS2
#include "ros2/pub.h"
#include "ros2/sub.h"
#endif

namespace banma {
namespace das {
namespace com {

    #ifdef USE_FASTDDS
    template <class MSG_T, class MSG_PUBSUB_T>
    FastDDSFactory<MSG_T, MSG_PUBSUB_T>::FastDDSFactory(const std::string& topic, uint32_t domain_id, bool reliable, bool large_data)
        : topic_(topic)
        , data_type_(new MSG_PUBSUB_T())
        , domain_id_(domain_id)
        , reliable_(reliable)
        , large_data_(large_data)
    {

    }

    template <class MSG_T, class MSG_PUBSUB_T>
    FastDDSFactory<MSG_T, MSG_PUBSUB_T>::~FastDDSFactory()
    {

    }

    template <class MSG_T, class MSG_PUBSUB_T>
    std::shared_ptr<IPublisher<MSG_T>> FastDDSFactory<MSG_T, MSG_PUBSUB_T>::CreatePubInst()
    {
        return std::shared_ptr<fast_dds::FastDDSPubImpl<MSG_T>>(new fast_dds::FastDDSPubImpl<MSG_T>(topic_, std::move(data_type_), domain_id_, reliable_, large_data_));
    }

    template <class MSG_T, class MSG_PUBSUB_T>
    std::shared_ptr<ISubscriber<MSG_T>> FastDDSFactory<MSG_T, MSG_PUBSUB_T>::CreateSubInst()
    {
        return std::shared_ptr<fast_dds::FastDDSSubImpl<MSG_T>>(new fast_dds::FastDDSSubImpl<MSG_T>(topic_, std::move(data_type_), domain_id_, reliable_, large_data_));
    }
    #endif // USE_FASTDDS


    // =========================================================================
    #ifdef USE_CYCLONEDDS
    template <class MessageT>
    CycloneDDSFactory<MessageT>::CycloneDDSFactory(const std::string& topic, uint32_t domain_id, bool reliable, \
        bool wait_listener, ::dds::sub::DataReaderListener<MessageT>* listener, \
        uint32_t max_duration_time_ms, bool large_data)

        : topic_(topic)
        , domain_id_(domain_id)
        , max_duration_time_ms_(max_duration_time_ms)
        , reliable_(reliable)
        , listener_(listener)
        , large_data_(large_data)
        , wait_listener_(wait_listener)
    {

    }

    template <class MessageT>
    CycloneDDSFactory<MessageT>::~CycloneDDSFactory()
    {

    }

    template <class MessageT>
    std::shared_ptr<IPublisher<MessageT>> CycloneDDSFactory<MessageT>::CreatePubInst()
    {
        return std::shared_ptr<cyclone_dds::CycloneDDSPubImpl<MessageT>>( \
            new cyclone_dds::CycloneDDSPubImpl<MessageT>(topic_, domain_id_, reliable_, wait_listener_, large_data_));
    }

    template <class MessageT>
    std::shared_ptr<ISubscriber<MessageT>> CycloneDDSFactory<MessageT>::CreateSubInst()
    {
        return std::shared_ptr<cyclone_dds::CycloneDDSSubImpl<MessageT>>( \
            new cyclone_dds::CycloneDDSSubImpl<MessageT>(topic_, domain_id_, reliable_, listener_, large_data_));
    }
    #endif // USE_CYCLONEDDS

} // namespace com
} // namespace das
} // namespace banma
