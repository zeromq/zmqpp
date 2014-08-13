#pragma once
#include <string>
#include <vector>

namespace zmqpp
{
  namespace z85
  {
    /**
     * Encode a binary string into a string using Z85 representation.
     * @param raw_data the binary string to be encoded.
     * @return the encoded string.
     * See ZMQ RFC 32;
     */
    std::string encode(const std::string &raw_data);

    /**
     * Encode a binary blob into a string using Z85 representation.
     * @param data pointer to raw data to be encoded.
     * @param size the size of the data to be encoded.
     * @return the encoded string.
     * See ZMQ RFC 32;
     */
    std::string encode(const uint8_t *data, size_t size);

    /**
     * Decode a Z85 encoded string into a binary blob represented as a vector.
     * @param string the string to be decoded.
     * @return a vector of uint8_t: the binary block after string decoding.
     */
    std::vector<uint8_t> decode(const std::string &string);
  };
};
