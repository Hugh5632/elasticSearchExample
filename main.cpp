#include <iostream>
#include <curl/curl.h>
#include <cpr/cpr.h>
#include "es_client.h"
#include "es_bulk_api.h"
#include <thread>
using namespace std;



//int main() {
//    CURL *curl = curl_easy_init();
//    if(curl){
//        CURLcode res;
//        curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:9200/test_index_apollo?pretty");
//        res = curl_easy_perform(curl);
//        curl_easy_cleanup(curl);
//    }
//    return 0;
//}



//int main(){
//    elasticlient::Client client({"127.0.0.1:9200/"});
//    cpr::Response index_response = client.index("test_index","_doc","","{}");
//    std::cout<< " code " << index_response.status_code<< std::endl;
//    std::cout<< " text "<< index_response.text<<std::endl;
//    cpr::Response get_response = client.get("test_index","_doc","1");
//    std::cout<< "status code "<<get_response.status_code <<" response " << get_response.text << std::endl;
//
//
//    return 0;
//}
struct Arg{
    shared_ptr<elasticlient::Client> client;
    int i ;

};
void create( void* arg){
    Arg *arg1 = (Arg*)arg;
    shared_ptr<elasticlient::Client> client = arg1->client;
    cpr::Response response =  client->performRequest(elasticlient::Client::HTTPMethod::GET,"mamba_dns_hx53_2019_09/_search?pretty","");
    std::cout<< arg1->i <<response.status_code << response.text<<std::endl;




}

int main(){

    timeval t1;
    timeval t2;
    Arg *arg = new Arg;
    shared_ptr<elasticlient::Client> client = std::make_shared<elasticlient::Client>(std::vector<std::string>({"http://127.0.0.1:9200/"}));
//    arg->client = client;
//    for(int i = 0 ; i < 5 ;++i){
//        arg-> i = i;
//        std::thread c(create,arg);
//        c.detach();
//    }
    gettimeofday(&t1, nullptr);
    elasticlient::Bulk bulk_index(client);
    elasticlient::EsBulkAPI bulk(10000);
    size_t errors;
    for(int i = 0 ; i < 10000; ++i){
        bulk.IndexDocument("create","test_bulk1","_doc","","{\"data\":\"data0\"}");
    }
    errors = bulk_index.Perform(bulk);
    std::cout << " errors" << errors<<std::endl;
    bulk.clear();
//GET mamba_session_hx53_2019_09/_search

//   cpr::Response response =  client ->performRequest(elasticlient::Client::HTTPMethod::GET,"mamba_dns_hx53_2019_09/_search?pretty","");
//   std::cout<< response.status_code << response.text<<std::endl;
////
    gettimeofday(&t2, nullptr);
    uint64_t  t = (t2.tv_sec -t1.tv_sec) * 1e6 + (t2.tv_usec - t1.tv_usec);
    std::cout<<" speed time " << t <<std::endl;
    std::cout<< bulk.Size() << " " << errors;






    return 0;
}

//#include <errno.h>
//#include <maxminddb.h>
//#include <stdlib.h>
//#include <string.h>
//#include <iostream>
//#include <glog/logging.h>
//
//using namespace std;

//int main(){
//    elasticlient::Client client({"192.168.100.43:9200/"});
//
//    elasticlient::Bulk bulk(client);
//    std::string doc_str{"{}"};
//    cpr::Response index_response = client.performRequest(elasticlient::Client::HTTPMethod::POST,"test_apollo/_doc","{}");
//    std::cout<< " code " << index_response.status_code<< std::endl;
//    std::cout<< " text "<< index_response.text<<std::endl;
////    cpr::Response get_response = client.get("test_index","_doc","1");
////    std::cout<< "status code "<<get_response.status_code <<" response " << get_response.text << std::endl;
//
//
//    return 0;
//}


//bool is_ip_str(const std::string & ip_str){
//    /* if string length less than 7, NOT a IP */
//    if ( ip_str.size() < 7 ) return false;
//    /* if start with . or end with 'a', NOT IP */
//    if ( ip_str.back() == '.' && ip_str.front() == '.') return false;
//    /* if dot number != 3, NOT IP string */
//    int dot_num = 0;
//    for(char c : ip_str){
//        if(c == '.')    dot_num++;
//        /* if there have character NOT number or '.', not IP */
//        if( !(!(c>='0' && c<='9') || (c != '.') )) {
//            return false;
//        }
//    }
//    /* if dot number != 3, NOT IP string */
//    if(dot_num != 3)
//        return false;
//
//    return true;
//}
//
//int main(int argc, char *argv[]){
////    google::InitGoogleLogging(argv[0]);
////    FLAGS_logtostderr = true;
//
//
//    timeval t1;
//    timeval t2;
//    std::shared_ptr<elasticlient::Client> client = std::make_shared<elasticlient::Client>(std::vector<std::string>({"http://192.168.100.43:9200/"}));
//    cpr::Response index_response = client->performRequest(elasticlient::Client::HTTPMethod::POST,"test_apollo/_alias/apollo_index?pretty",""); //创建索引，用别名指向它

//    const char* str = "{ \
//            \"aliases\":{ \
//            \"mamba_session_2021-03\":{ \
//            \"is_write_index\":true \
//    } \
//} \
//}";
//
//    cpr::Response index_response = client->performRequest(elasticlient::Client::HTTPMethod::PUT,"%3Cmamba_%7Bnow%2Fd%7D-000001%3E",str);
//    std::cout<< " test "<< index_response.status_code<< index_response.text<<endl;
//
//  const char* alias_json = "{ \
//          \"aliases\": { \
//          \"mamba_session_hx53_2019_10\": { \
//          \"is_write_index\": true \
//  } \
//} \
//}";
//    cpr::Response response = client->performRequest(elasticlient::Client::HTTPMethod::PUT, "mamba_session_hx53_2019_10-000001", alias_json);
//    std::cout<< " test  "<< response.status_code<< response.text<<endl;
//    bool is_next_month = true;
//    if(is_next_month) {
//        cpr::Response index_response = client->performRequest(elasticlient::Client::HTTPMethod::PUT, "session_log_000001", alias_json);
//        is_next_month = false;
//    }
//    cpr::Response index_response = client->performRequest(elasticlient::Client::HTTPMethod::GET, "_cat/indices/session_log", "");
//        std::cout<< " test rollover "<< index_response.status_code<< index_response.text<<endl;
//        cpr::Response response = client->performRequest(elasticlient::Client::HTTPMethod::GET,"session_log/_search?pretty",{});
//    std::cout<< " rollover "<< response.status_code<< response.text<<endl;
//
//    elasticlient::Bulk bulk_index(client);
//    elasticlient::SameIndexBulkData bulk("mamba_session_hx53_2019_10",100);
//    size_t errors;
//    for(int i = 0 ; i < 10; ++i){
//        bulk.createDocument("_doc","","{\"data\":\"data0\"}");
//    }
//    errors = bulk_index.perform(bulk);
//    std::cout<< bulk.size()<<" "<<errors<<std::endl;
//    cpr::Response response = client->performRequest(elasticlient::Client::HTTPMethod::GET, "mamba_session_hx53_2019_10/_search", "");
//    std::cout<< " test  "<< response.status_code<< response.text<<endl;




//    const char* json_rollover ="{ \
//            \"conditions\":{ \
//            \"max_age\":\"7d\", \
//            \"max_docs\":10000, \
//            \"max_size\":\"20G\" \
//    } \
//}";
//    response = client->performRequest(elasticlient::Client::HTTPMethod::POST,"mamba_session_hx53_2019_10/_rollover",json_rollover);
//    std::cout<< " test rollover "<< response.status_code<< response.text<<endl;

//    gettimeofday(&t1, nullptr);
//    elasticlient::SameIndexBulkData bulk("test_bulk",100);
//    size_t errors;
//    for(int i = 0 ; i < 100; ++i){
//        bulk.indexDocument("_doc","","{\"data\":\"data0\"}");
//        bulk.indexDocument("_doc","","{\"data\":\"data1\"}");
//        bulk.indexDocument("_doc","","{\"data\":\"data2\"}");
//        bulk.indexDocument("_doc","","{\"data\":\"data3\"}");
//        bulk.indexDocument("_doc","","{\"data\":\"data4\"}");
//    }
//    errors = bulk_index.perform(bulk);
//
//    gettimeofday(&t2, nullptr);
//    uint64_t  t = (t2.tv_sec -t1.tv_sec) * 1e6 + (t2.tv_usec - t1.tv_usec);
//    std::cout<<" speed time " << t <<std::endl;
//    std::cout << bulk.size() << " " << errors;
//    bulk.clear();


//uint64_t  speed_time;
//gettimeofday(&session_ptr->session_->start_timing_,nullptr);
//
//gettimeofday(&session_ptr->session_->end_timing_,nullptr);
//speed_time = (session_ptr->session_->end_timing_.tv_sec -session_ptr->session_->start_timing_.tv_sec) * 1e6 + (session_ptr->session_->end_timing_.tv_usec - session_ptr->session_->start_timing_.tv_usec);
//DLOG(INFO) <<" speed_time " << speed_time << endl;
//
//gettimeofday(&session_ptr->session_->end_timing_,nullptr);
//speed_time = (session_ptr->session_->end_timing_.tv_sec -session_ptr->session_->start_timing_.tv_sec) * 1e6 + (session_ptr->session_->end_timing_.tv_usec - session_ptr->session_->start_timing_.tv_usec);
//DLOG(INFO) <<" speed_time " << speed_time << endl;

//
//    return 0;
//}

//
//int main(int argc, char **argv)
//{
////    char *filename = argv[1];
//    char *filename = "/home/elastic/project/cmake-build-debug-apollo/GeoLite2-City_20210316/GeoLite2-City.mmdb";
//    char *ip_address = "220.181.12.100";
//
//    MMDB_s mmdb;
//    int status = MMDB_open(filename, MMDB_MODE_MMAP, &mmdb);
//
//    if (MMDB_SUCCESS != status) {
//        fprintf(stderr, "\n  Can't open %s - %s\n",
//                filename, MMDB_strerror(status));
//
//        if (MMDB_IO_ERROR == status) {
//            fprintf(stderr, "    IO error: %s\n", strerror(errno));
//        }
//        exit(1);
//    }
//
//    int gai_error, mmdb_error;
//    MMDB_lookup_result_s result =
//            MMDB_lookup_string(&mmdb, ip_address, &gai_error, &mmdb_error);
//
//    if (0 != gai_error) {
//        fprintf(stderr,
//                "\n  Error from getaddrinfo for %s - %s\n\n",
//                ip_address, gai_strerror(gai_error));
//        exit(2);
//    }
//
//    if (MMDB_SUCCESS != mmdb_error) {
//        fprintf(stderr,
//                "\n  Got an error from libmaxminddb: %s\n\n",
//                MMDB_strerror(mmdb_error));
//        exit(3);
//    }
//
//    MMDB_entry_data_list_s *entry_data_list = NULL;
//
//    int exit_code = 0;
//    cout << 1 << endl;
//    if (result.found_entry) {
//        int status = MMDB_get_entry_data_list(&result.entry,
//                                              &entry_data_list);
//        cout << 1.1 << endl;
//
//        if (MMDB_SUCCESS != status) {
//            fprintf(
//                    stderr,
//                    "Got an error looking up the entry data - %s\n",
//                    MMDB_strerror(status));
//            exit_code = 4;
//            goto end;
//        }
//
//        if (NULL != entry_data_list) {
//            MMDB_dump_entry_data_list(stdout, entry_data_list, 2);
//        }
//        cout << 1.2 << endl;
//
//    } else {
//        fprintf(
//                stderr,
//                "\n  No entry for this IP address (%s) was found\n\n",
//                ip_address);
//        exit_code = 5;
//    }
//
//
//
//end:
//    MMDB_free_entry_data_list(entry_data_list);
//    MMDB_close(&mmdb);
//    exit(exit_code);
//}
