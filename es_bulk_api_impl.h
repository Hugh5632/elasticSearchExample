//
// Created by chiyuen on 2021/3/30.
//

#ifndef HX_ES_BULK_API_IMPL_H
#define HX_ES_BULK_API_IMPL_H

#include "es_bulk_api.h"
#include <string>
#include <vector>
#include <ostream>
#include <stdexcept>

namespace elasticlient{

    struct BulkItem{
        // control field such as create delete get
        std::string control;
        //data field such as {"_index":"website"."_type":"_doc","_id":"123"}
        std::string source;
        BulkItem(const std::string &control = "", const std::string &source = ""):control(control),source(source){}

        //友元函数对操作符 << 进行重载
        friend std::ostream &operator<<(std::ostream &os, const BulkItem &item){
            if(item.control.empty()){
                return os;
            }
            os<< item.control << "\n";
            if(not item.source.empty()){
                os<< item.source<<"\n";
            }
            return os;

        }

    };

    //{"index": {"_index":"index1","_type": "type1", "_id": ""}}
    std::string CreateControl( const std::string &action,const std::string &index, const std::string &doc_type, const std::string &doc_id);

    class EsBulkAPI::Implementation{
        // Index to which all data belongs to.
        std::string index_name_;
        /// Desired bulk size
        std::size_t size_;
        // Bulk documents
        std::vector<BulkItem> data_{};

    public:
        Implementation(const std::string &index_name, std::size_t size):index_name_(index_name),size_(size){
            if(index_name.empty()){
                throw::std::runtime_error("Index name is mandatory argument");
            }
            if(size){
                data_.reserve(size);//重新分配vector的容量的大小 只有在当程序所申请的容量大于vector的当前容量，才会为vector重新分配存储空间
            }
        }
        Implementation(std::size_t size):size_(size){
            if(size){
                data_.resize(size);
            }
        }

        friend class EsBulkAPI;


    };


    class Bulk::Implementation{
        /// Client holder
        std::shared_ptr<Client> client;
        /// Number of errors occured (failed to index).
        std::size_t errCount;
        // 允许bulk 访问私有成员变量
        friend class Bulk;

        // 检查 bulk结果的正确性并更新错误计数器
        void processResult(const std::string &result, std::size_t size);
    public:
        Implementation(std::shared_ptr<Client> elastic_client)
                : client(std::move(elastic_client)), errCount(0)
        {
            if (!client) {
                throw std::runtime_error("Valid Client instance is required.");
            }
        }

        /**
         * Send bulk body on Client
         * Request errors are counted to the bulk counters.
         */
        void run(const IBulkData &bulk);

    };



}


#endif //HX_ES_BULK_API_IMPL_H
