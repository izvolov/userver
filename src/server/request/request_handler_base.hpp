#pragma once

#include <boost/optional.hpp>

#include <components/component_context.hpp>
#include <logging/logger.hpp>
#include <server/handlers/handler_base.hpp>
#include <server/request/request_base.hpp>

#include <engine/task/task_processor.hpp>
#include "request_task.hpp"

namespace server {
namespace request {

class RequestHandlerBase {
 public:
  RequestHandlerBase(
      const components::ComponentContext& component_context,
      const boost::optional<std::string>& logger_access_component,
      const boost::optional<std::string>& logger_access_tskv_component);
  virtual ~RequestHandlerBase() {}

  virtual std::shared_ptr<RequestTask> PrepareRequestTask(
      std::shared_ptr<RequestBase>&& request,
      std::function<void()>&& notify_func) const = 0;
  virtual void ProcessRequest(RequestTask& task) const = 0;

  const components::ComponentContext& GetComponentContext() const;

  const logging::LoggerPtr& LoggerAccess() const { return logger_access_; }
  const logging::LoggerPtr& LoggerAccessTskv() const {
    return logger_access_tskv_;
  }

 private:
  logging::LoggerPtr logger_access_;
  logging::LoggerPtr logger_access_tskv_;
};

}  // namespace request
}  // namespace server
