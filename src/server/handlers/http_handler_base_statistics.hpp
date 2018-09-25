#pragma once

#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <vector>

#include <server/handlers/http_handler_base.hpp>
#include <utils/statistics.hpp>
#include <utils/statistics/percentile.hpp>
#include <utils/statistics/recentperiod.hpp>

namespace server {
namespace handlers {

class ReplyCodesStatistics {
 public:
  std::unordered_map<unsigned int, size_t> GetReplyCodes() const {
    std::unordered_map<unsigned int, size_t> codes;

    for (size_t i = 1; i < reply_codes_100_.size(); i++) {
      const size_t count = reply_codes_100_[i];
      codes[i * 100] = count;
    }

    codes[401] = reply_401_;

    return codes;
  }

  void Account(unsigned int code) {
    if (code == 401) reply_401_++;
    if (code < 100) code = 100;
    int code_100 = std::min<size_t>(code / 100, reply_codes_100_.size() - 1);
    reply_codes_100_[code_100]++;
  }

 private:
  std::array<std::atomic<size_t>, 6> reply_codes_100_{};
  std::atomic<size_t> reply_401_{0};
};

class HttpHandlerBase::Statistics {
 public:
  void Account(unsigned int code, size_t ms) {
    reply_codes_.Account(code);
    timings_.GetCurrentCounter().Account(ms);
  }

  std::unordered_map<unsigned int, size_t> GetReplyCodes() const {
    return reply_codes_.GetReplyCodes();
  }

  using Percentile = utils::statistics::Percentile<2048, unsigned int, 120>;

  Percentile GetTimings() const { return timings_.GetCurrentCounter(); }

 private:
  utils::statistics::RecentPeriod<Percentile, Percentile,
                                  utils::datetime::SteadyClock>
      timings_;
  ReplyCodesStatistics reply_codes_;
};

class HttpHandlerBase::HandlerStatistics {
 public:
  Statistics& GetStatisticByMethod(::http_method method);

  Statistics& GetTotalStatistics();

  void Account(http_method method, unsigned int code,
               std::chrono::milliseconds ms);

  const std::vector<http_method>& GetAllowedMethods() const;

 private:
  Statistics stats_;
  std::array<Statistics, 5> stats_by_method_;
};

}  // namespace handlers
}  // namespace server
