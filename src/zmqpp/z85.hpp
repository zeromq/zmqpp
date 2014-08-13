#pragma once
#include <string>
#include <vector>

namespace zmqpp
{
  namespace z85
  {
    std::string encode(const std::string &raw_data);
    std::string encode(const uint8_t *data, size_t size);

    std::vector<uint8_t> decode(const std::string &string);
  };
};
