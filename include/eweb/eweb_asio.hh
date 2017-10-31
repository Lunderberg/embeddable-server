#pragma once

#ifdef ASIO_STANDALONE

#include <system_error>
#include <asio.hpp>

namespace eweb {
namespace asio = ::asio;
using error_code = std::error_code;
}


#else

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace eweb {
namespace asio = boost::asio;
using error_code = boost::system::error_code;
}

#endif
