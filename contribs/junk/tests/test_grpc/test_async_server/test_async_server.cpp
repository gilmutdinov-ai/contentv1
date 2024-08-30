#include "contribs/junk/tests/test_grpc/cpp_grpc_library/greeter.grpc.pb.h"
#include "contribs/junk/tests/test_grpc/cpp_grpc_library/greeter.pb.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

// Имеено async без лишних плагинов
// https://github.com/grpc/grpc/blob/master/examples/cpp/helloworld/greeter_async_server.cc

// Из офф доки
// https://grpc.io/docs/languages/cpp/async/#async-server

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
/*
class GreeterServiceImpl final : public greeter::Greeter::Service {
  grpc::Status SayHello(grpc::ServerContext *context,
                        const greeter::HelloRequest *request,
                        greeter::HelloReply *reply) override {
    std::string prefix("Hello from test_grpc.cpp");
    reply->set_message(prefix + request->name());
    return grpc::Status::OK;
  }
};
*/
class ServerImpl final {
public:
  ~ServerImpl() {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
  }

  // There is no shutdown handling in this code.
  void Run(uint16_t port) {
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);

    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    cq_ = builder.AddCompletionQueue();
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Proceed to the server's main loop.
    HandleRpcs();
  }

private:
  // Class encompassing the state and logic needed to serve a request.
  class CallData {
  public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    CallData(greeter::Greeter::AsyncService *service, ServerCompletionQueue *cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      // Invoke the serving logic right away.
      Proceed();
    }

    void Proceed() {
      if (status_ == CREATE) {
        // Make this instance progress to the PROCESS state.
        status_ = PROCESS;

        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.
        service_->RequestSayHello(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
      } else if (status_ == PROCESS) {
        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        new CallData(service_, cq_);

        // The actual processing.
        std::string prefix("_prefix_from_server_:");
        reply_.set_message(prefix + request_.name());

        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
      } else {
        // REQUIRE(status_ == FINISH);
        //  Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
      }
    }

  private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    greeter::Greeter::AsyncService *service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue *cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    greeter::HelloRequest request_;
    // What we send back to the client.
    greeter::HelloReply reply_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<greeter::HelloReply> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_; // The current serving state.
  };

  // This can be run in multiple threads if needed.
  void HandleRpcs() {
    // Spawn a new CallData instance to serve new clients.
    new CallData(&service_, cq_.get());
    void *tag; // uniquely identifies a request.
    bool ok;
    while (true) {
      // Block waiting to read the next event from the completion queue. The
      // event is uniquely identified by its tag, which in this case is the
      // memory address of a CallData instance.
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or cq_ is shutting down.
      cq_->Next(&tag, &ok);
      // REQUIRE(ok);
      static_cast<CallData *>(tag)->Proceed();
    }
  }

  std::unique_ptr<ServerCompletionQueue> cq_;
  greeter::Greeter::AsyncService service_;
  std::unique_ptr<Server> server_;
};

int main(int argc, char **argv) {

  ServerImpl server;
  int port = 8082;
  server.Run(port);

  /*
   Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const
   ::grpc::StubOptions& options = ::grpc::StubOptions());
      ::grpc::Status SayHello(::grpc::ClientContext* context, const
   ::greeter::HelloRequest& request, ::greeter::HelloReply* response) override;
  */
  return 0;
}
