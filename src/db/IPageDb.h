#pragma once

#include "core/Types.h"
#include <functional>
#include <memory>

namespace contentv1 {

class IPageDb {

public:
  using Ptr = std::shared_ptr<IPageDb>;

  virtual ~IPageDb() = default;

  virtual void save(const Url &_url, const std::string &_content) = 0;
  virtual bool load(const Url &_url, std::string &_content) = 0;

  using ScanPageCb = std::function<void(const Url &, const std::string &)>;
  virtual void scanFull(ScanPageCb _cb) = 0;
};

} // namespace contentv1