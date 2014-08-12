#include "signal.hpp"

namespace std
{

    ostream &operator<<(ostream &s, const zmqpp::signal &sig)
    {
        s << static_cast<int64_t> (sig);
        return s;
    }
}