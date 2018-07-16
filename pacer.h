#pragma once

#include <stdint.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "exceptions.h"

class pacer
{
public:

  pacer() { }

  pacer(const pacer &) = delete;

  pacer operator=(const pacer &) = delete;

  pacer(pacer &&) = delete;

  pacer operator=(pacer &&) = delete;

  ~pacer()
  {
    close();
  }

  void set_period_ns(long int nanoseconds)
  {
    if (fd == -1)
    {
      fd = timerfd_create(CLOCK_MONOTONIC, 0);
      if (fd == -1)
      {
        throw posix_error("Failed to create timerfd");
      }
    }

    struct itimerspec spec = { {0, nanoseconds}, {0, 1} };
    int result = timerfd_settime(fd, 0, &spec, NULL);
    if (result == -1)
    {
      throw posix_error("Failed to set timerfd interval");
    }
  }

  uint64_t pace()
  {
    uint64_t expirations = 0;
    ssize_t result = read(fd, &expirations, sizeof(expirations));
    if (result != 8)
    {
      throw std::runtime_error("Failed to read from timer.");
    }
    return expirations;
  }

  void close()
  {
    if (fd != -1)
    {
      ::close(fd);
      fd = -1;
    }
  }

private:
  int fd = -1;
};
