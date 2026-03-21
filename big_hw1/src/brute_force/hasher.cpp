#include "brute_force/hasher.hpp"
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace brute_force_node::brute_force {
namespace {
std::string to_hex(const unsigned char *data, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
    }
    return oss.str();
}
}  // namespace

std::string get_hash(const std::string &value) {
    unsigned char md5_result[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char *>(value.data()), value.size(),
        md5_result);
    unsigned char sha1_result[SHA_DIGEST_LENGTH];
    SHA1(md5_result, MD5_DIGEST_LENGTH, sha1_result);

    return to_hex(sha1_result, SHA_DIGEST_LENGTH);
}

}  // namespace brute_force_node::brute_force