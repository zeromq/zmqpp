#include <boost/test/unit_test.hpp>
#include "zmqpp/z85.hpp"
#include "zmqpp/exception.hpp"


#if (ZMQ_VERSION_MAJOR >= 4)

BOOST_AUTO_TEST_SUITE( z85 )

BOOST_AUTO_TEST_CASE(test_encode)
{
  uint8_t data_source[] = {0x86, 0x4F, 0xD2, 0x6F, 0xB5, 0x59, 0xF7, 0x5B};
  std::string res;

  res = zmqpp::z85::encode(data_source, sizeof(data_source));
  BOOST_CHECK_EQUAL(res, "HelloWorld");

  std::string src(reinterpret_cast<char *>(data_source), sizeof(data_source));
  res = zmqpp::z85::encode(src);
  BOOST_CHECK_EQUAL(res, "HelloWorld");
}

BOOST_AUTO_TEST_CASE(test_decode)
{
  std::string data_source = "HelloWorld";

  std::vector<uint8_t> ret;

  ret = zmqpp::z85::decode(data_source);
  std::vector<uint8_t> expected = {0x86, 0x4F, 0xD2, 0x6F, 0xB5, 0x59, 0xF7, 0x5B};

  BOOST_CHECK_EQUAL(ret.size(), expected.size());
  for (unsigned int i = 0; i < expected.size(); ++i)
    BOOST_CHECK_EQUAL(ret[i], expected[i]);
}

BOOST_AUTO_TEST_CASE(test_wrong_size)
{
  std::string encode_source = "123"; // not divisible by 4
  std::string decode_source = "123456"; // not divisible by 5
  

  BOOST_CHECK_THROW(zmqpp::z85::encode(encode_source), zmqpp::z85_exception);
  BOOST_CHECK_THROW(zmqpp::z85::decode(decode_source), zmqpp::z85_exception);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
