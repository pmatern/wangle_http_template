#include "gtest/gtest.h"
#include "http.h"

class MockContext : public wangle::Handler<std::string, Request, Response, std::string>::Context
{
public:
    void fireRead(Request msg) override {
        result_ = msg;
    }

    void fireReadEOF() override {}
    void fireReadException(folly::exception_wrapper e) override {}
    void fireTransportActive() override {}
    void fireTransportInactive() override {}
    folly::Future<folly::Unit> fireWrite(std::string msg) override {
        return folly::Future<folly::Unit>{};
    }
    folly::Future<folly::Unit> fireWriteException(folly::exception_wrapper e) override {
        return folly::Future<folly::Unit>{};
    }
    folly::Future<folly::Unit> fireClose() override {
        return folly::Future<folly::Unit>{};
    }
    wangle::PipelineBase* getPipeline() override {
        return nullptr;
    }
    std::shared_ptr<wangle::PipelineBase> getPipelineShared() override {
        return std::shared_ptr<wangle::PipelineBase>{};
    }
    void setWriteFlags(folly::WriteFlags flags) override {}
    folly::WriteFlags getWriteFlags() override {
        return folly::WriteFlags{};
    }
    void setReadBufferSettings(uint64_t minAvailable, uint64_t allocationSize) override {}
    std::pair<uint64_t, uint64_t> getReadBufferSettings() override {
        return std::pair<uint64_t, uint64_t>{};
    }

    Request get_result()
    {
        return result_;
    }

private:
    Request result_;
};

std::vector<std::string> make_get_request() {
    std::vector<std::string> request = {
        "GET /path/to/resource HTTP/1.1\r\n",
        "Host: www.awesome.com\r\n\r\n"
    };
    return std::move(request);
}

std::vector<std::string> make_post_request() {
    std::string body = "parameter%3Dvalue%26also%3Danother";
    std::vector<std::string> request = {
        "POST /path/to/resource HTTP/1.1\r\n",
        "Host: www.awesome.com\r\n",
        "Content-Type: application/x-www-form-urlencoded\r\n",
        std::string{"Content-Length: "} + std::to_string(body.length()) + "\r\n\r\n",
        body
    };
    return std::move(request);
}

void testPostRequest(HttpCodec& codec, MockContext& context) {
    for (auto line : make_post_request()) {
        codec.read(&context, line);
    }
    
    auto result = context.get_result();

    EXPECT_EQ("POST", result.method_string().to_string());
    EXPECT_EQ("www.awesome.com", result["Host"].to_string());
    EXPECT_EQ("/path/to/resource", result.target()); 
    EXPECT_EQ("parameter%3Dvalue%26also%3Danother", result.body());   
}

void testGetRequest(HttpCodec& codec, MockContext& context) {
    for (auto line : make_get_request()) {
        codec.read(&context, line);
    }
    
    auto result = context.get_result();

    EXPECT_EQ("GET", result.method_string().to_string());
    EXPECT_EQ("www.awesome.com", result["Host"].to_string());
    EXPECT_EQ("/path/to/resource", result.target());    
}


TEST(http_codec_test, parse_get) {
    HttpCodec codec{};
    MockContext context{};

    testGetRequest(codec, context);
    //tests parser reset between requests
    testGetRequest(codec, context);
}


TEST(http_codec_test, parse_post) {
    HttpCodec codec{};
    MockContext context{};

    testPostRequest(codec, context);
    //tests parser reset between requests
    testPostRequest(codec, context);
}

 
int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}