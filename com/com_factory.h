/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/
#ifndef _DAS_COM_FACTORY_H_
#define _DAS_COM_FACTORY_H_

#include <memory>
#include "com_intf.h"

#ifdef USE_FASTDDS
#include <fastdds/dds/topic/TopicDataType.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#endif // USE_FASTDDS

#ifdef USE_CYCLONEDDS
#include <dds/dds.hpp>
#endif // USE_CYCLONEDDS


namespace banma {
namespace das {
namespace com {

    template <class MessageT>
    class IFactory {
    public:
        IFactory() {};
        IFactory(const IFactory& other) = default;
        IFactory(IFactory&& other) = default;
        IFactory& operator=(const IFactory& other) = default;
        IFactory& operator=(IFactory&& other) = default;
        
        virtual ~IFactory() {};
        virtual std::shared_ptr<IPublisher<MessageT>> CreatePubInst() = 0;
        virtual std::shared_ptr<ISubscriber<MessageT>> CreateSubInst() = 0;
    };

    // =========================================================================
    #ifdef USE_FASTDDS

    using namespace eprosima::fastdds::dds;

    template <class MSG_T, class MSG_PUBSUB_T>
    class FastDDSFactory : public IFactory<MSG_T> {
    public:
        explicit FastDDSFactory(const std::string& topic, uint32_t domain_id = 0, bool reliable = false, bool large_data = false);
        FastDDSFactory(const FastDDSFactory& other) = default;
        FastDDSFactory(FastDDSFactory&& other) = default;
        FastDDSFactory& operator=(const FastDDSFactory& other) = default;
        FastDDSFactory& operator=(FastDDSFactory&& other) = default;
        virtual ~FastDDSFactory();
        virtual std::shared_ptr<IPublisher<MSG_T>> CreatePubInst() override;
        virtual std::shared_ptr<ISubscriber<MSG_T>> CreateSubInst() override;
    private:
        std::string topic_;
        TypeSupport data_type_;
        uint32_t domain_id_;
        bool reliable_;
        bool large_data_;
    };
    #endif // USE_FASTDDS

    // =========================================================================
    #ifdef USE_CYCLONEDDS
    template <class MessageT>
    class CycloneDDSFactory : public IFactory<MessageT> {
    public:
        explicit CycloneDDSFactory(const std::string& topic, uint32_t domain_id = 0, bool reliable = false, \
            bool wait_listener = false, ::dds::sub::DataReaderListener<MessageT>* listener = nullptr, \
            uint32_t max_duration_time_ms = 100, bool large_data = true);
        CycloneDDSFactory(const CycloneDDSFactory& other) = default;
        CycloneDDSFactory(CycloneDDSFactory&& other) = default;
        CycloneDDSFactory& operator=(const CycloneDDSFactory& other) = default;
        CycloneDDSFactory& operator=(CycloneDDSFactory&& other) = default;
        virtual ~CycloneDDSFactory();
        virtual std::shared_ptr<IPublisher<MessageT>> CreatePubInst() override;
        virtual std::shared_ptr<ISubscriber<MessageT>> CreateSubInst() override;
    private:
        std::string topic_;
        uint32_t domain_id_;
        uint32_t max_duration_time_ms_;
        bool reliable_;
        ::dds::sub::DataReaderListener<MessageT>* listener_;
        bool large_data_;
        bool wait_listener_;
    };
    #endif // USE_CYCLONEDDS

} // namespace com
} // namespace das
} // namespace banma

#endif // _DAS_COM_FACTORY_H_
