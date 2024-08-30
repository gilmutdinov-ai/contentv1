#include "scheduler/rpc/SchedulerClient.h"
#include "misc/Log.h"

namespace contentv1 {

//// TRY FETCH URLS
void SchedulerClient::tryFetchUrls(const std::vector<GFQRecord> &_try_urls,
                                   std::vector<GFQRecord> &_allowed_urls) {
  tryFetchUrls_(_try_urls, _allowed_urls, false);
}

void SchedulerClient::tryFetchUrls_(const std::vector<GFQRecord> &_try_urls,
                                    std::vector<GFQRecord> &_allowed_urls,
                                    bool _async) {
  TryFetchRequest g_request;
  for (const auto &gfq : _try_urls) {
    auto g_gfq = g_request.add_try_urls();
    gfq.toPb(g_gfq);
  }
  TryFetchReply g_reply;
  callTryFetchUrlsSync_(g_request, g_reply);
  const auto &g_allowed_urls = g_reply.allowed_urls();
  for (const auto &gfq_pb : g_allowed_urls) {
    _allowed_urls.push_back(GFQRecord::fromPb(gfq_pb));
  }
}

void SchedulerClient::callTryFetchUrlsSync_(TryFetchRequest &_g_request,
                                            TryFetchReply &_g_reply) {
  ClientContext context;
  // The actual RPC.
  Status status = stub_->TryFetchUrls(&context, _g_request, &_g_reply);

  if (!status.ok()) {
    LOG(status.error_code() << ": " << status.error_message());
  }
}

/// ON FETCHED
void SchedulerClient::onFetched(const std::vector<FetchResult> &_results) {

  OnFetchedRequest g_request;
  for (const auto &result : _results) {
    auto g_result = g_request.add_results();
    result.toPb(g_result);
  }
  OnFetchedReply g_reply;

  ClientContext context;
  // The actual RPC.
  Status status = stub_->OnFetched(&context, g_request, &g_reply);

  if (!status.ok()) {
    LOG(status.error_code() << ": " << status.error_message());
  }
}

} // namespace contentv1
