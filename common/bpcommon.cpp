#include <sstream>
#include <chrono>
#include "bpcommon.hpp"

namespace bp {

/* 枚举是字符串 enum2str 传true */
void JsonPbConvert::PbMsg2JsonStr(const ProtobufMsg& src, std::string& dst, bool enum2str) {
    Json::Value value;
    PbMsg2Json(src, value, enum2str);
    Json::FastWriter writer;
    dst = writer.write(value);
}

bool JsonPbConvert::JsonStr2PbMsg(const std::string& src, ProtobufMsg& dst, bool str2enum) {
    Json::Value value;
    Json::Reader reader(Json::Features::strictMode());
    if (!reader.parse(src, value)) {
        return false;
    }
    if (true != Json2PbMsg(value, dst, str2enum)) {
        return false;
    }
    return true;
}

bool JsonPbConvert::Json2PbMsg(const Json::Value& src, ProtobufMsg& dst, bool str2enum) {
    const ProtobufDescriptor* descriptor = dst.GetDescriptor();
    const ProtobufReflection* reflection = dst.GetReflection();
    if (nullptr == descriptor || nullptr == reflection) {
        return false;
    }
 
    int32_t count = descriptor->field_count();
    for (int32_t i = 0; i < count; ++i) {
        const ProtobufFieldDescriptor* field = descriptor->field(i);
        if (nullptr == field) {
            continue;
        }
 
        if (!src.isMember(field->name())){
            continue;
        }

        const Json::Value& value = src[field->name()];
        if (field->is_repeated()) {
            if (!value.isArray()) {
                return false;
            } else {
                Json2RepeatedMessage(value, dst, field, reflection, str2enum);
                continue;
            }
        }
        switch (field->type()) {
            case ProtobufFieldDescriptor::TYPE_BOOL: {
                if (value.isBool()) {
                    reflection->SetBool(&dst, field, value.asBool());
                } else if (value.isInt()) {
                    reflection->SetBool(&dst, field, value.isInt());
                } else if (value.isString()) {
                    if (value.asString() == "true") {
                        reflection->SetBool(&dst, field, true);
                    } else if (value.asString() == "false") {
                        reflection->SetBool(&dst, field, false);
                    }
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32: {
                if (value.isInt()){
                    reflection->SetInt32(&dst, field, value.asInt());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32: {
                if (value.isUInt()){
                    reflection->SetUInt32(&dst, field, value.asUInt());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64: {
                if (value.isInt()) {
                    reflection->SetInt64(&dst, field, value.asInt64());
                }
                break;
            } 
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64: {
                if (value.isUInt()) {
                    reflection->SetUInt64(&dst, field, value.asUInt64());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_FLOAT: {
                if (value.isDouble()) {
                    reflection->SetFloat(&dst, field, value.asFloat());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_DOUBLE: {
                if (value.isDouble()) {
                    reflection->SetDouble(&dst, field, value.asDouble());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES: {
                if (value.isString()) {
                    reflection->SetString(&dst, field, value.asString());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_MESSAGE: {
                if (value.isObject()) {
                    Json2PbMsg(value, *reflection->MutableMessage(&dst, field));
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    return true;
}

void JsonPbConvert::PbMsg2Json(const ProtobufMsg& src, Json::Value& dst, bool enum2str) {
    const ProtobufDescriptor* descriptor = src.GetDescriptor();
    const ProtobufReflection* reflection = src.GetReflection();
    if (nullptr == descriptor || nullptr == descriptor) {
        return;
    }
 
    int32_t count = descriptor->field_count();
    for (int32_t i = 0; i < count; ++i) {
        const ProtobufFieldDescriptor* field = descriptor->field(i);
        if (field->is_repeated()) {
            if (reflection->FieldSize(src, field) > 0) {
                RepeatedMessage2Json(src, field, reflection, dst[field->name()], enum2str);
            }
            continue;
        }
        // if (!reflection->HasField(src, field)) {
        //     continue;
        // }
 
        switch (field->type()) {
            case ProtobufFieldDescriptor::TYPE_MESSAGE: {
                const ProtobufMsg& tmp_message = reflection->GetMessage(src, field);
                if (0 != tmp_message.ByteSize()) {
                    PbMsg2Json(tmp_message, dst[field->name()]);
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_BOOL: {
                dst[field->name()] = reflection->GetBool(src, field) ? true : false;
                break;
            }
            case ProtobufFieldDescriptor::TYPE_ENUM: {
                const ::google::protobuf::EnumValueDescriptor* enum_value_desc = reflection->GetEnum(src, field);
                if (enum2str) {
                    dst[field->name()] = enum_value_desc->name();
                } else {
                    dst[field->name()] = enum_value_desc->number();
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32: {
                dst[field->name()] = Json::Int(reflection->GetInt32(src, field));
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32: {
                dst[field->name()] = Json::UInt(reflection->GetUInt32(src, field));
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64: {
                dst[field->name()] = Json::Int64(reflection->GetInt64(src, field));
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64: {
                dst[field->name()] = Json::UInt64(reflection->GetUInt64(src, field));
                break;
            }
            case ProtobufFieldDescriptor::TYPE_FLOAT: {
                dst[field->name()] = reflection->GetFloat(src, field);
                break;
            }
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES: {
                dst[field->name()] = reflection->GetString(src, field);
                break;
            }
            default: {
                break;
            }
        }
    }
}
 
bool JsonPbConvert::Json2RepeatedMessage(const Json::Value& json, ProtobufMsg& message, 
                                const ProtobufFieldDescriptor* field,
                                const ProtobufReflection* reflection,
                                bool str2enum) {
    int32_t count = json.size();
    for (int32_t j = 0; j < count; ++j) {
        switch (field->type()) {
            case ProtobufFieldDescriptor::TYPE_BOOL: {
                if (json.isBool()) {
                    reflection->AddBool(&message, field, json[j].asBool());
                } else if (json[j].isInt()) {
                    reflection->AddBool(&message, field, json[j].asInt());
                } else if (json[j].isString()) {
                    if (json[j].asString() == "true") {
                        reflection->AddBool(&message, field, true);
                    } else if (json[j].asString() == "false") {
                        reflection->AddBool(&message, field, false);
                    }
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_ENUM: {
                const ::google::protobuf::EnumDescriptor *pedesc = field->enum_type();
                const ::google::protobuf::EnumValueDescriptor* pevdesc = nullptr;
                if (str2enum) {
                    pevdesc = pedesc->FindValueByName(json[j].asString());
                } else {
                    pevdesc = pedesc->FindValueByNumber(json[j].asInt());
                }
                if (nullptr != pevdesc) {
                    reflection->AddEnum(&message, field, pevdesc);
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32: {
                if (json[j].isInt()) {
                    reflection->AddInt32(&message, field, json[j].asInt());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32: {
                if (json[j].isUInt()) {
                    reflection->AddUInt32(&message, field, json[j].asUInt());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64: {
                if (json[j].isInt()) {
                    reflection->AddInt64(&message, field, json[j].asInt64());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64: {
                if (json[j].isUInt()) { 
                    reflection->AddUInt64(&message, field, json[j].asUInt64());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_FLOAT: {
                if (json[j].isDouble()) {
                    reflection->AddFloat(&message, field, json[j].asFloat());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_DOUBLE: {
                if (json[j].isDouble()) {
                    reflection->AddDouble(&message, field, json[j].asDouble());
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_MESSAGE: {
                if (json[j].isObject()) {
                    Json2PbMsg(json[j], *reflection->AddMessage(&message, field));
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES: {
                if (json[j].isString()) {
                    reflection->AddString(&message, field, json[j].asString());
                }
                break;
            }
            default: {
                break;
            }
        }
    }
    return true;
}

void JsonPbConvert::RepeatedMessage2Json(const ProtobufMsg& message, 
                                        const ProtobufFieldDescriptor* field,
                                        const ProtobufReflection* reflection, 
                                        Json::Value& json, bool enum2str) {
    if (nullptr == field || nullptr == reflection) {
        PbMsg2Json(message, json);
    }
 
    for (int32_t i = 0; i < reflection->FieldSize(message, field); ++i) {
        Json::Value tmp_json;
        switch (field->type()) {
            case ProtobufFieldDescriptor::TYPE_MESSAGE: {
                const ProtobufMsg& tmp_message = reflection->GetRepeatedMessage(message, field, i);
                if (0 != tmp_message.ByteSize()) {
                    PbMsg2Json(tmp_message, tmp_json);
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_BOOL:
                tmp_json[field->name()] = reflection->GetRepeatedBool(message, field, i) ? true : false;
                break;
            case ProtobufFieldDescriptor::TYPE_ENUM: {
                const ::google::protobuf::EnumValueDescriptor* enum_value_desc = reflection->GetRepeatedEnum(message, field, i);
                if (enum2str) 
                {
                    tmp_json = enum_value_desc->name();
                } 
                else 
                {
                    tmp_json = enum_value_desc->number();
                }
                break;
            }
            case ProtobufFieldDescriptor::TYPE_INT32:
            case ProtobufFieldDescriptor::TYPE_SINT32:
            case ProtobufFieldDescriptor::TYPE_SFIXED32:
                tmp_json[field->name()] = reflection->GetRepeatedInt32(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_UINT32:
            case ProtobufFieldDescriptor::TYPE_FIXED32:
                tmp_json[field->name()] = reflection->GetRepeatedUInt32(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_INT64:
            case ProtobufFieldDescriptor::TYPE_SINT64:
            case ProtobufFieldDescriptor::TYPE_SFIXED64:
                tmp_json[field->name()] = (Json::Int64)reflection->GetRepeatedInt64(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_UINT64:
            case ProtobufFieldDescriptor::TYPE_FIXED64:
                tmp_json[field->name()] = Json::UInt64(reflection->GetRepeatedUInt64(message, field, i));
                break;
            case ProtobufFieldDescriptor::TYPE_FLOAT:
                tmp_json[field->name()] = reflection->GetRepeatedFloat(message, field, i);
                break;
            case ProtobufFieldDescriptor::TYPE_STRING:
            case ProtobufFieldDescriptor::TYPE_BYTES:
                tmp_json[field->name()] = reflection->GetRepeatedString(message, field, i);
                break;
            default:
                break;
        }
        json.append(tmp_json);
    }
}

bool set_attr(std::shared_ptr<::google::protobuf::Message> msg, const std::string& json_str) {
    return JsonPbConvert::JsonStr2PbMsg(json_str, *msg);
}

bool get_attr(const std::shared_ptr<::google::protobuf::Message> msg, std::string& json_str) {
    JsonPbConvert::PbMsg2JsonStr(*msg, json_str);
    return true;
}

std::string BpCommon::GetModName(const std::string& full_path) {
    std::stringstream ss(full_path);
    std::string mod_name;
    std::getline(ss, mod_name, '.');
    return mod_name;
}

std::string BpCommon::GetCurTime() {
    auto now = std::chrono::system_clock::now();
    uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
        - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);
    char str_time[25] = { 0 };
    sprintf(str_time, "%02d:%02d:%02d.%03d", time_tm->tm_hour,
        time_tm->tm_min, time_tm->tm_sec, (int)dis_millseconds);
    return std::string(str_time);
}

} // namespace bp