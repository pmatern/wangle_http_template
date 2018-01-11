#include "http.h"
#include <glog/logging.h>

using namespace boost::beast;

class write_cb
{       
public:
    std::string capture_;
    bool invoked;
    
    template<class ConstBufferSequence>
    void operator()(error_code& ec, ConstBufferSequence const& buffers)
    {
        invoked = true;
        ec.assign(0, ec.category());
        for (auto buf : buffers) {
            std::size_t size = boost::asio::buffer_size(buf);
            const char* data = boost::asio::buffer_cast<const char*>(buf);
            std::string chunk{data, size};
            capture_ += chunk;
        }  
    }
};


RequestParser& HttpCodec::parser_ref() {
    if (!parser_ptr) {
        parser_ptr = new RequestParser{};
        parser_ptr->eager(true);
    }
    return *parser_ptr;
}

Request HttpCodec::finish_parse() {
    auto parsed = parser_ref().release();

    delete parser_ptr;
    parser_ptr = nullptr;

    unparsed_buffer_.clear();

    return parsed;
}


void HttpCodec::read(Context* ctx, const std::string line) {
    LOG(INFO) << "received line " << line;

    RequestParser& parser = parser_ref();
    
    unparsed_buffer_ += line;

    auto buf = boost::asio::buffer(unparsed_buffer_);
    boost::beast::error_code ec;
    auto bytes_read = parser.put(buf, ec);
    
    if (ec &&  http::error::need_more != ec) {
        LOG(ERROR) << "http parser failed with message " << ec.message();
        finish_parse();
        ctx->fireClose();
    }

    if (unparsed_buffer_.length() == bytes_read) {
        unparsed_buffer_.clear();
    } else if (bytes_read > 0) {
        unparsed_buffer_ = unparsed_buffer_.substr(bytes_read, unparsed_buffer_.length()-bytes_read);
    }

    if (parser.is_done()) {
        auto parsed = finish_parse();
        ctx->fireRead(std::move(parsed));
    }
}


folly::Future<folly::Unit> HttpCodec::write(Context* ctx, const Response resp) {
    ResponseSerializer serializer{resp};
    boost::beast::error_code ec;
    std::string serialized;

    while (!serializer.is_done()) {
        write_cb cb;
        serializer.next(ec, cb);

        if (cb.invoked) {
            serialized += cb.capture_;
            serializer.consume(cb.capture_.length());
        }
    }

    LOG(INFO) << "writing response: " << serialized;
    return ctx->fireWrite(std::move(serialized));
}
