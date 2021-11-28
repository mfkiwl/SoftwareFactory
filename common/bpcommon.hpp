#ifndef __BPCOMMON_HPP__
#define __BPCOMMON_HPP__
#include <memory>
#include <string>
#include <jsoncpp/json/json.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace bp {

    class JsonPbConvert {
    public:
        typedef ::google::protobuf::Message         ProtobufMsg;
        typedef ::google::protobuf::Reflection      ProtobufReflection;
        typedef ::google::protobuf::FieldDescriptor ProtobufFieldDescriptor;
        typedef ::google::protobuf::Descriptor      ProtobufDescriptor;
    
    public:
        /* 枚举是字符串 enum2str 传true */
        static void PbMsg2JsonStr(const ProtobufMsg& src, std::string& dst, bool enum2str = false);
        static bool JsonStr2PbMsg(const std::string& src, ProtobufMsg& dst, bool str2enum = false);
        
        static void PbMsg2Json(const ProtobufMsg& src, Json::Value& dst, bool enum2str = false);
        static bool Json2PbMsg(const Json::Value& src, ProtobufMsg& dst, bool str2enum = false);
    
    private:
        static bool Json2RepeatedMessage(const Json::Value& json, ProtobufMsg& message, 
                                        const ProtobufFieldDescriptor* field,
                                        const ProtobufReflection* reflection,
                                        bool str2enum = false);
        
        static void RepeatedMessage2Json(const ProtobufMsg& message, 
                                                const ProtobufFieldDescriptor* field,
                                                const ProtobufReflection* reflection, 
                                                Json::Value& json, bool enum2str);
    };

    bool set_attr(std::shared_ptr<::google::protobuf::Message> msg, const std::string& json_str);

    bool get_attr(const std::shared_ptr<::google::protobuf::Message> msg, std::string& json_str);

}; // namespace bp

#endif