#pragma once

#include <wangle/channel/Handler.h>
#include <boost/beast/http.hpp>
#include <boost/optional.hpp>

using Request = boost::beast::http::message<true, boost::beast::http::string_body>;
using Response = boost::beast::http::message<false, boost::beast::http::string_body>;
using RequestParser = boost::beast::http::request_parser<boost::beast::http::string_body>;
using ResponseSerializer = boost::beast::http::response_serializer<boost::beast::http::string_body>;


class HttpCodec : public wangle::Handler<std::string, Request, Response, std::string> {
public: 
    HttpCodec() = default;
    HttpCodec(const HttpCodec& ) = default;
    HttpCodec& operator= (const HttpCodec & ) = default;
    HttpCodec& operator= (HttpCodec&& other) = default;
    ~HttpCodec()
    {
        if (parser_ptr) {
            delete parser_ptr;
        }
    }; 

    typedef typename Handler<std::string, Request, Response, std::string>::Context Context;

    void read(Context* ctx, std::string line) override; 

    folly::Future<folly::Unit> write(Context* ctx, Response resp) override;
private:
    RequestParser& parser_ref();
    Request finish_parse();
    
    std::string unparsed_buffer_;
    RequestParser *parser_ptr;
};
