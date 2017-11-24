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

namespace QYJ{
struct noncopyable {
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};
}
class Item : QYJ::noncopyable {
   public:
    enum UpdatePolicy {
        kInvalid,   //值无效
        kSet,   //设置键值
        kAdd,   //增加键
        kReplace,   //替换键
        kAppend,    //追加键值
        kPrepend,   //在前面追加键值
        kCas,   //设置编号
    };

    static ItemPtr makeItem(StringPiece keyarg, uint32_t flagsArg,
                            int exptimeArg, int valuelen, uint64_t casArg) {
        return std::make_shared<Item>(keyarg, flagsArg, exptimeArg, valuelen,
                                      casArg);
    }

    Item(StringPiece keyArg, uint32_t flagsArg, int exptimeArg, int valuelen,
         uint64_t casArg);
    ~Item() { ::free(data_); }

    StringPiece key() const { return StringPiece(data_, keylen_); } //返回键

    uint32_t flags() const { return flags_; }

    int rel_exptime() const { return rel_exptime_; }

    const char* value() const { return data_ + keylen_; }
    size_t valueLength() const { return valuelen_; }

    uint64_t cas() const { return cas_; }
    size_t hash() const { return hash_; }
    void setCas(uint64_t casArg) { cas_ = casArg; }
    size_t neededBytes() const { return totalLen() - receivedBytes_; }
    void append(const char* data, size_t len);

    bool endsWithCRLF() const { //判断是否crlf结尾
        return receivedBytes_ == totalLen() && data_[totalLen() - 2] == '\r' &&
               data_[totalLen() - 1] == '\n';
    }
    void output(muduo::net::Buffer* out, bool needCas = false) const;
    void resetKey(StringPiece k);   //重置键

   private:
    int totalLen() const { return keylen_ + valuelen_; }    //总长度
    int keylen_;    //键长度
    const uint32_t flags_  ;    //参数    
    const int rel_exptime_; //超时时间
    const int valuelen_;    //值长度
    int receivedBytes_; //接收字节数
    uint64_t cas_;  //编号
    size_t hash_;   //hash值
    char* data_;    //值
};

#endif