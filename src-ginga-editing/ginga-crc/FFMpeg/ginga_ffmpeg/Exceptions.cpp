#include "Exceptions.h"

namespace br {
namespace ufscar {
namespace lince {
namespace util {

FileWriteException::FileWriteException()
{
    message = "FileWriteException: ";
}

FileWriteException::~FileWriteException() throw()
{

}

const char*
FileWriteException::what() const throw()
{
    return message.c_str();
}

StreamWriteException::StreamWriteException()
{
    message = "StreamWriteException: ";
}

StreamWriteException::~StreamWriteException() throw()
{

}

const char*
StreamWriteException::what() const throw()
{
    return message.c_str();
}

EncodingException::EncodingException()
{
    message = "EncodingException: ";
}

EncodingException::~EncodingException() throw()
{

}

const char*
EncodingException::what() const throw()
{
    return message.c_str();
}

NetworkingException::NetworkingException()
{
    message = "NetworkingException: ";
}

NetworkingException::~NetworkingException() throw()
{

}

const char*
NetworkingException::what() const throw()
{
    return message.c_str();
}

URIException::URIException()
{
    message = "URIException: ";
}

URIException::~URIException() throw()
{

}

const char*
URIException::what() const throw()
{
    return message.c_str();
}

}
}
}
}

