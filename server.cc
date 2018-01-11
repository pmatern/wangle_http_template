#include "server.h"
#include <glog/logging.h>

using namespace folly;
using namespace wangle;
using namespace boost::beast::http;

void EchoHandler::read(Context* ctx, Request req) {
    LOG(INFO) << "received request for " << req.target();
    
    Response resp{status::ok, 11, req.body()};
    
    for (auto iter = req.begin(); iter != req.end(); ++iter) {
        resp.set(iter->name(), iter->value());
    }

    resp.set(field::server, "echo-server");
    resp.keep_alive(req.keep_alive());
    if (!req.has_content_length()) {
        resp.content_length(0);
    }

    write(ctx, resp);
}

class GreedyInputDecoder : public InboundHandler<IOBufQueue&, std::unique_ptr<IOBuf>> {
public:
    void read(Context* ctx, IOBufQueue& q) override {
        ctx->fireRead(std::move(q.move()));
    }
};

EchoPipeline::Ptr EchoPipelineFactory::newPipeline(std::shared_ptr<AsyncTransportWrapper> sock) {
    auto pipeline = EchoPipeline::create();
    pipeline->addBack(AsyncSocketHandler{sock});
    pipeline->addBack(GreedyInputDecoder{});
    pipeline->addBack(StringCodec{});
    pipeline->addBack(HttpCodec{});
    pipeline->addBack(EchoHandler{});
    pipeline->finalize();
    return pipeline;
}

