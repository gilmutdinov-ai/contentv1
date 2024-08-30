#pragma once

#include "scheduler/SchedulerApi.h"

#include "src/scheduler/rpc/scheduler.grpc.pb.h"
#include "src/scheduler/rpc/scheduler.pb.h"

#include <grpcpp/grpcpp.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

// #include "absl/flags/flag.h"
// #include "absl/flags/parse.h"
// #include "absl/log/check.h"

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

namespace contentv1 {

class SchedulerClient : public SchedulerApi {
public:
  explicit SchedulerClient(std::shared_ptr<Channel> channel)
      : stub_(SchedulerRPC::NewStub(channel)) {}

  /*void tryFetchUrlsSync(const std::vector<GFQRecord> &_try_urls,
                        std::vector<GFQRecord> &_allowed_urls); */

  void tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                    std::vector<GFQRecord> &_allowed_urls) override;
  void onFetched(const std::vector<FetchResult> &_results) override;

private:
  void tryFetchUrls_(const std::vector<GFQRecord> &_try_urls,
                     std::vector<GFQRecord> &_allowed_urls, bool _async);

  // void callTryFetchUrlsAsync_(TryFetchRequest &_g_request,
  //                             TryFetchReply &_g_reply);

  void callTryFetchUrlsSync_(TryFetchRequest &_g_request,
                             TryFetchReply &_g_reply);

private:
  std::unique_ptr<SchedulerRPC::Stub> stub_;
};
/*
int main(int argc, char **argv) {
  std::string target_str = "127.0.0.1:8082";
  GreeterClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::string user("_client_argument_");
  std::string reply = greeter.SayHello(user); // The actual RPC call!
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
*/
} // namespace contentv1
