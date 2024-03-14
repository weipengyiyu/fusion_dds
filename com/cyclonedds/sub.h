/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#ifndef _DAS_CYCLONEDDS_SUB_H_
#define _DAS_CYCLONEDDS_SUB_H_

#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <dds/dds.hpp>

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
    class CycloneDDSSubImpl : public ISubscriber<MessageT> {
    public:
        explicit CycloneDDSSubImpl(const std::string& topic, const uint32_t& domain_id = 0, const bool& reliable = false, \
            ::dds::sub::DataReaderListener<MessageT>* listener = nullptr, const bool& large_data = true);
        CycloneDDSSubImpl(const CycloneDDSSubImpl& other) = default;
        CycloneDDSSubImpl(CycloneDDSSubImpl&& other) = default;
        CycloneDDSSubImpl& operator=(const CycloneDDSSubImpl& other) = default;
        CycloneDDSSubImpl& operator=(CycloneDDSSubImpl&& other) {if (this != &other) {*this = other; return *this;}};
        virtual ~CycloneDDSSubImpl();

        virtual bool Init(const std::string& cfg_file_path = "") override;
        virtual COM_ERRCODE Subscribe(std::unique_ptr<MessageT>& msg) override;

    private:
        ::dds::sub::DataReader<MessageT> data_reader_;
        ::dds::sub::LoanedSamples<MessageT> samples_;
        typename ::dds::sub::LoanedSamples<MessageT>::const_iterator sample_iter_;
        bool large_data_;
        
    private:
        ::dds::sub::DataReader<MessageT> MakeDataReader(const std::string& topic_str, const uint32_t& domain_id, const bool& reliable, ::dds::sub::DataReaderListener<MessageT> *listener);
    };


} // namespace cyclone_dds
} // namespace com
} // namespace das
} // namespace banma



#endif //_DAS_CYCLONEDDS_SUB_H_