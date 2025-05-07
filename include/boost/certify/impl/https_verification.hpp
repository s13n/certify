#ifndef BOOST_CERTIFY_IMPL_HTTPS_VERIFICATION_HPP
#define BOOST_CERTIFY_IMPL_HTTPS_VERIFICATION_HPP

#include <boost/certify/https_verification.hpp>

namespace boost
{
namespace certify
{

template<class NextLayer>
void
set_server_hostname(asio::ssl::stream<NextLayer>& stream,
                    std::string_view hostname,
                    std::error_code& ec)
{
    detail::set_server_hostname(stream.native_handle(), hostname, ec);
}

template<class NextLayer>
void
set_server_hostname(asio::ssl::stream<NextLayer>& stream, std::string_view hostname)
{
    std::error_code ec;
    certify::set_server_hostname(stream, hostname, ec);
    if (ec)
        boost::throw_exception(std::system_error{ec});
}

} // namespace certify
} // namespace boost

#endif // BOOST_CERTIFY_IMPL_HTTPS_VERIFICATION_HPP
