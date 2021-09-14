//
// Created by chiyuen on 2021/3/30.
//

#include "es_bulk_api.h"
#include "es_bulk_api_impl.h"
#include "es_client.h"
#include <string>
#include <sstream>
#include <cpr/cpr.h>
#include <rapidjson/document.h>
#include <iostream>

namespace {
    //检查文档是否包含换行符
    void validateDocument(const std::string &doc, const std::string &id) {
        // 文档中不可出现换行符
        if (doc.find_first_of("\n") != std::string::npos) {
//            DLOG(ERROR)<< "A document for %s contains newline character.", id.c_str())<<endl;
            throw std::runtime_error("Cannot index document containing newline char");
        }
    }

}// anonymous namespace


namespace elasticlient{

    IBulkData::~IBulkData() {}

    EsBulkAPI::EsBulkAPI(const std::string &index_name, std::size_t size): impl(new Implementation(index_name, size)){

    }

    EsBulkAPI::EsBulkAPI(std::size_t size):impl(new Implementation(size)) {

    }

    EsBulkAPI::~EsBulkAPI() {


    }

    std::string EsBulkAPI::IndexName() const {
        return impl->index_name_;
    }

    bool EsBulkAPI::Empty() const {
        return impl->data_.empty();
    }

    std::string EsBulkAPI::Body() const {
        std::ostringstream body;
        for(const auto &one:impl->data_){
            body<<one;
        }
        return body.str();
    }

    std::size_t EsBulkAPI::Size() const {
        return impl->data_.size();
    }

    bool EsBulkAPI::IndexDocument(const std::string &action,const std::string &index, const std::string &doc_type, const std::string &id, const std::string &doc) {
        validateDocument(doc,id);
        impl->data_.emplace_back(CreateControl(action,index,doc_type,id),doc);
        return impl->data_.size() >= impl->size_;
    }

    void EsBulkAPI::clear() {
        impl->data_.resize(0);
    }

    Bulk::Bulk(const std::shared_ptr<Client> &client)
            : impl(new Implementation(client))
    {}

    Bulk::Bulk(const std::vector<std::string> &host_url_list, std::int32_t connection_timeout):impl(new Implementation(std::make_shared<Client>(host_url_list,connection_timeout))) {}

    Bulk::~Bulk() {}

    Bulk::Bulk(Bulk &bulk) {

    }

    Bulk::Bulk(Bulk &&other) = default;

    std::string CreateControl( const std::string &action,const std::string &index, const std::string &doc_type, const std::string &doc_id){
        std::ostringstream out;
        if(doc_id.empty()){
            out << "{\"" << action << "\": {""\"_index\": \""<<index<<"\"," "\"_type\": \"" << doc_type << "\"}}";
        } else{
            out << "{\"" << action << "\": {""\"_index\": \""<<index<<"\"," "\"_type\": \"" << doc_type << "\"," "\"_id\": \"" << doc_id << "\" }}";
        }
        return out.str();
    }

    void Bulk::Implementation::run(const IBulkData &bulk) {
        std::string body = bulk.Body();
        std::string index_name = bulk.IndexName();
        try{
            const cpr::Response r= client->performRequest(Client::HTTPMethod::POST,"/_bulk",body);
            if(r.status_code /100 != 2){
                throw ConnectionException("Elastic node not response with status 2xx.");
            }
            processResult(r.text,bulk.Size());
        } catch (const ConnectionException) {
            errCount+=bulk.Size();
        }
    }

    std::size_t Bulk::Perform(const IBulkData &bulk) {

        if (bulk.Empty()) { return 0; }

//        LOG(LogLevel::INFO, "Going to index %lu elements.", bulk.size());
        impl->errCount = 0;
        impl->run(bulk);
        return impl->errCount;

    }

    void Bulk::Implementation::processResult(const std::string &result, std::size_t size){

        rapidjson::Document root;
        rapidjson::ParseResult ok = root.Parse(result.c_str());
        if (!ok || !root.IsObject()) {
            // probably whole bulk has failed
            errCount += size;
            return;
        }

        // Expected response:
        // {"took": int,
        //  "errors": false,
        //  "items": [
        //      {"index": {
        //          "_index": string,
        //          "_type": string,
        //          "_id": string,
        //          "_version": int,
        //          "_shards": {"total": int, "successful": int, "failed": int},
        //          "status": 201}}
        //  ]}

        // check errors flag, if it is false, do not parse single responses
        if (root.HasMember("errors")) {
            const rapidjson::Value &errors = root["errors"];
            if (errors.IsBool() && !errors.GetBool()) {
                // everything is alright, errors==false.
                return;
            }
        }

        // check presence of the bulk items results
        if (!root.HasMember("items")) {
//            LOG(LogLevel::WARNING, "Bulk ran with errors, but no items are present "
//                                   "at the response! Err count is inaccurate now!");
            return;
        }
        const rapidjson::Value &items = root["items"];
        // check correct type of the items
        if (!items.IsArray()) {
//            LOG(LogLevel::WARNING, "Failed to read elastic response field 'items', because "
//                                   "it is not an array! Err count is inaccurate now!");
            return;
        }

        // process items responses
        for (const rapidjson::Value &item: items.GetArray()) {
            if (!item.IsObject()) {
//                LOG(LogLevel::WARNING, "Bulk items responses have to be objects!");
                errCount++;
                continue;
            }

            // check index action response
            if (item.HasMember("index")) {
                const rapidjson::Value &res = item["index"];
                if (!res.IsObject()) {
//                    LOG(LogLevel::WARNING, "Bulk response has unexpected format, "
//                                           "object was expected.");
                    errCount++;
                    continue;
                }
                // read status code
                if (!res.HasMember("status")) {
//                    LOG(LogLevel::WARNING, "Bulk response item with missing status.");
                    errCount++;
                    continue;
                };
                const rapidjson::Value &status = res["status"];
                if (!status.IsInt()) {
//                    LOG(LogLevel::WARNING, "Bulk response was expected to have numeric status. "
//                                           "Skipping this response checking.");
                    errCount++;
                    continue;
                }

                // if status code is not 2xx family, consider it as error
                if (status.GetInt() / 100 != 2) {
                    errCount++;
                }
            } else {
//                LOG(LogLevel::WARNING, "Unsupported 'action' found at bulk response.");
            }
        }

        // complain if not all items of the bulk were covered by responses
        if (items.Size() < size) {
//            LOG(LogLevel::INFO, "Bulk has more items than responses received. Cannot tell "
//                                "whether %lu items succeeded...", size - items.Size());
        }

    }

    std::size_t Bulk::GetErrorCount() const {
        return 0;
    }

    const std::shared_ptr<Client> &Bulk::GetClient() const {
//        return <#initializer#>;
    }


} // namespace elasticlient




