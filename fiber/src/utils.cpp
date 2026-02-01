#include "utils.h"

namespace wbfiber {
pid_t GetThreadId() { return syscall(SYS_gettid); }

u_int32_t GetFiberId() {
  // TODO
  return 0;
}
}  // namespace wbfiber