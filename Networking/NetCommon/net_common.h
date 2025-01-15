#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <iostream>
#include <chrono>
#include <memory>
#include <mutex>
#include <deque>
#include <vector>
#include <thread>
#include <algorithm>
#include <cstdint>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


