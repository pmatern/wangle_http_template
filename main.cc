#include <wangle/bootstrap/ServerBootstrap.h>
#include "app_config.h"
#include "server.h"

using namespace wangle;

DEFINE_int32(port, 8080, "port");

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    LOG(INFO) << argv[0] << " version " << app_VERSION_MAJOR << "." << app_VERSION_MINOR;

    ServerBootstrap<EchoPipeline> server;
    server.childPipeline(std::make_shared<EchoPipelineFactory>());
    server.bind(FLAGS_port);
    server.waitForStop();

    return 0;
}
