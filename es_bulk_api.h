//
// Created by chiyuen on 2021/3/30.
//

#ifndef HX_ES_BULK_API_H
#define HX_ES_BULK_API_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace elasticlient{

    class Client;

    // bulk 内单条索引数据
    class IBulkData{
    public:

        virtual ~IBulkData();
        /**
         * 返回批量数据所属的索引名称
         * 如果数据具有不同的索引名称 则为空
         */
        virtual std::string IndexName() const = 0;

        //判断bulk中是否有数据
        virtual bool Empty() const = 0;

        //返回bulk中索引数量
        virtual std::size_t Size() const = 0;

        // 返回bulk中索引数据
        virtual std::string Body() const = 0;

    };
    // 组成bulk解析
    class EsBulkAPI: public IBulkData {
        class Implementation;
        std::unique_ptr<Implementation> impl;

    public:

        EsBulkAPI(const std::string &index_name, std::size_t size = 100);

        EsBulkAPI(std::size_t size = 100);

        ~EsBulkAPI() override;

        std::string IndexName() const override;

        bool Empty() const override;

        std::string Body() const override;

        std::size_t Size() const override;

        // 添加 index请求 到Bulk
        bool IndexDocument(const std::string &action, const std::string &index,const std::string &docType, const std::string &id,const std::string &doc);

        void clear();

    };

    //写入 bulk
    class Bulk{

        class Implementation;
        std::unique_ptr<Implementation> impl;
    public:

         // 初始化Bulk
         Bulk(const std::shared_ptr<Client> &client);

        /**
         * 初始化Bulk索引 并为指定的对象创建Client实例
         * hostUrlList and connectionTimeout.
         * \param hostUrlList list of URLs of Elastic nodes in one cluster.
         * \param connectionTimeout Elasticsearch node connection timeout.
         */
        Bulk(const std::vector<std::string> &hostUrlList,
             std::int32_t connectionTimeout);

        ~Bulk();

        Bulk(Bulk &&other);

        //赋值拷贝函数
        Bulk(Bulk &bulk);

        /**
         * 执行Bulk
         * 返回错误发生数量
         */
        std::size_t Perform(const IBulkData &bulk);

        /// Return number of errors in last bulk being ran.
        std::size_t GetErrorCount() const;

        /// 返回具有当前配置的Client类
        const std::shared_ptr<Client> &GetClient() const;

    };

} // namespace elasticlient

#endif //HX_ES_BULK_API_H




