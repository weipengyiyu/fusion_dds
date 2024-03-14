/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/

#ifndef _DAS_COM_INTF_H_
#define _DAS_COM_INTF_H_

#include <string>
#include <memory>


namespace banma {
namespace das {
namespace com {

    enum COM_ERRCODE {
        COM_OK = 0,
        COM_PUB_FAILED,
        COM_PUB_NO_LISTENER,
        COM_SUB_FAILED,
        COM_SUB_NO_PUBLISHER,
    };

    template<class MessageT>
    class IPublisher {
    public:
        IPublisher(const std::string& topic, uint32_t domain_id = 0) : topic_name_(topic), domain_id_(domain_id) {};
        virtual ~IPublisher() {};
        virtual bool Init(const std::string& cfg_file_path = "") = 0;
        virtual COM_ERRCODE Publish(MessageT& msg) = 0;
        std::string GetTopicName() {return this->topic_name_;}
    protected:
        std::string topic_name_;
        uint32_t domain_id_; // should < 232
    };

    template<class MessageT>
    class ISubscriber {
    public:
        ISubscriber(const std::string& topic, uint32_t domain_id = 0) : topic_name_(topic), domain_id_(domain_id) {}
        virtual ~ISubscriber() {}
        virtual bool Init(const std::string& cfg_file_path = "") = 0;
        virtual COM_ERRCODE Subscribe(std::unique_ptr<MessageT>& msg) = 0;
        std::string GetTopicName() {return this->topic_name_;}
    protected:
        std::string topic_name_;
        uint32_t domain_id_; // should < 232
    };

} // namespace com
} // namespace das
} // namespace banma

#endif //_DAS_COM_INTF_H_
