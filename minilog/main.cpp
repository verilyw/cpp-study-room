
#include "minilog.h"

int main()
{
  minilog::log_trace("hello world {}", 42);
  minilog::log_debug("hello world {}", 42); 
  minilog::log_info("hello world {}", 42); 
  minilog::log_warning("hello world {}", 42); 
  minilog::log_error("hello world {}", 42);
  minilog::log_fatal("hello world {}", 42);
  return 0;
}
