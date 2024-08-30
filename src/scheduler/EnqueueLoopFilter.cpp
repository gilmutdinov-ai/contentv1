#include "EnqueueLoop.h"
#include "misc/Log.h"
#include "misc/Time.h"
#include "misc/Vectors.h"

namespace contentv1 {

std::vector<EUrlCrawlDecisionPb>
EnqueueLoop::_needCrawlUrls(const std::vector<UrlFreq> &_in_urls,
                            std::vector<UrlFreq> &_out_urls) {
  // сходить в hostdb, проверить robots
  // сходить в urldb, там решат надо ли
  Tp now = std::chrono::system_clock::now();
  std::vector<UrlFreq> allowed_urls;
  std::vector<UrlFreq> need_robots;
  m_host_db->leaveAllowedByRobots(_in_urls, allowed_urls, need_robots);

  std::vector<UrlFreq> need_urls;
  auto decisions = m_url_db->leaveNeedUrls(allowed_urls, now, need_urls);

  // leave unique need_robots (add freqs)
  std::vector<UrlFreq> need_robots_uniq;
  reduceUrlFreqs(need_robots, need_robots_uniq);

  // filter need_robots by robots policy
  std::vector<UrlFreq> need_robots_uniq_filtered;
  m_url_db->leaveNeedRobots(need_robots_uniq, now, need_robots_uniq_filtered);

  // write need_robots, need_urls
  _out_urls.clear();
  _out_urls.reserve(need_robots_uniq_filtered.size() + need_urls.size());
  _out_urls.insert(_out_urls.end(), need_robots_uniq_filtered.begin(),
                   need_robots_uniq_filtered.end());
  _out_urls.insert(_out_urls.end(), need_urls.begin(), need_urls.end());
  return decisions;
}

std::vector<EUrlCrawlDecisionPb>
EnqueueLoop::_filter(Cnt _top, std::vector<UrlFreq> &_push_urls) {
  std::vector<UrlFreq> top_urls_freq;
  m_url_freq_stats->getTopUrls(top_urls_freq, _top);
  return _needCrawlUrls(top_urls_freq, _push_urls);
}

std::vector<EUrlCrawlDecisionPb>
EnqueueLoop::searchTop(Cnt push_target, std::vector<UrlFreq> &push_urls) {
  //
  //  filter(top_cur) -> push_size
  //
  //  find top_cur that
  //    push_size_min <= push_size
  //
  //   and then limit result to push_target
  //
  auto tp_start = misc::get_now();
  LOG("top search start: " << misc::to_readable(tp_start));
  LOG("m_url_freq_stats->max(): " << m_url_freq_stats->max());
  Cnt push_size_min = push_target;
  Cnt push_size_cur = -1;
  Cnt top_cur = push_target;
  std::vector<EUrlCrawlDecisionPb> decisions;

  auto update_filter = [&]() {
    decisions = _filter(top_cur, push_urls);
    push_size_cur = push_urls.size();
  };

  ////// FIND top_max (x CEILING)
  update_filter();
  while (push_size_cur < push_size_min) {
    top_cur *= 2;
    update_filter();
    LOG("top_cur: " << top_cur << " push_size_cur: " << push_size_cur);
    if (push_size_cur >= push_size_min)
      break;

    if (top_cur > m_url_freq_stats->max()) {
      LOG("cannot up ceiling, not enough urls");
      return decisions;
    }
  }

  if (push_urls.size() > push_target) {
    std::vector<UrlFreq> push_urls_limited;
    misc::limit_vector(push_urls, push_urls_limited, push_target);
    push_urls = std::move(push_urls_limited);
  }
  return decisions;
}

} // namespace contentv1
