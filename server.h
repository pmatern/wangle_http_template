#pragma once

#include <wangle/bootstrap/ServerBootstrap.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/codec/LineBasedFrameDecoder.h>
#include <wangle/codec/StringCodec.h>
#include "http.h"

using EchoPipeline =  wangle::Pipeline<folly::IOBufQueue&, Response>;

class EchoHandler : public wangle::HandlerAdapter<Request, Response> {
public:
    virtual void read(Context* ctx, Request req) override;
};

class EchoPipelineFactory : public wangle::PipelineFactory<EchoPipeline> {
public:
    EchoPipeline::Ptr newPipeline(std::shared_ptr<folly::AsyncTransportWrapper> sock);
};