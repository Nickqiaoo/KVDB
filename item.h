#ifndef ITEM_H__
#define ITEM_H__

#include <muduo/base/Atomic.h>
#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>

#include <memory>

using muduo::StringPiece;
using muduo::string;

namespace muduo {
namespace net {
class Buffer;
}
}  // namespace muduo

class Item;

using ItemPtr = std::shared_ptr<Item>;
using ConstItemPtr = std::shared_ptr<const Item>;

struct noncopyable {
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};

class Item : noncopyable {
   public:
    enum UpdatePolicy {
        kInvalid,
        kSet,
        kAdd,
        kReplace,
        kAppend,
        kPrepend,
        kCas,
    };

    static ItemPtr makeItem(StringPiece keyarg, uint32_t flagsArg,
                            int exptimeArg, int valuelen, uint64_t casArg) {
        return std::make_shared<Item>(keyarg, flagsArg, exptimeArg, valuelen,
                                      casArg);
    }

    Item(StringPiece keyArg, uint32_t flagsArg, int exptimeArg, int valuelen,
         uint64_t casArg);
    ~Item() { ::free(data_); }

    StringPiece key() const { return StringPiece(data_, keylen_); }

    uint32_t flags() const { return flags_; }

    int rel_exptime() const { return rel_exptime_; }

    const char* value() const { return data_ + keylen_; }
    size_t valueLength() const { return valuelen_; }

    uint64_t cas() const { return cas_; }
    size_t hash() const { return hash_; }
    void setCas(uint64_t casArg) { cas_ = casArg; }
    size_t neededBytes() const { return totalLen() - receivedBytes_; }
    void append(const char* data, size_t len);

    bool endsWithCRLF() const {
        return receivedBytes_ == totalLen() && data_[totalLen() - 2] == '\r' &&
               data_[totalLen() - 1] == '\n';
    }
    void output(muduo::net::Buffer* out, bool needCas = false) const;
    void resetKey(StringPiece k);

   private:
    int totalLen() const { return keylen_ + valuelen_; }
    int keylen_;
    const uint32_t flags_;
    const int rel_exptime_;
    const int valuelen_;
    int receivedBytes_;
    uint64_t cas_;
    size_t hash_;
    char* data_;
};

#endif