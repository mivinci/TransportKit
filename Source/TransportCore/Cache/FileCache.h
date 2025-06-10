#pragma once
#include "TransportCore/Def.h"
#include "TransportCore/Result.h"

namespace TP {
class FileCache {
 public:
  explicit FileCache(int32_t task_id) : task_id_(task_id) {}

  TP_SSIZE readClip(int32_t clip_no, int32_t start, int32_t end, char *buf,
                    size_t buf_size);
  TP_SSIZE writeClip(int32_t clip_no, int32_t start, int32_t end, char *buf,
                     size_t buf_size);
  TP_RESULT flushClip(int32_t clip_no);

 private:
  int32_t task_id_;
};
}  // namespace TP
