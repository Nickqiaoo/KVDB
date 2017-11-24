#ifndef SESSION_H__
#define SESSION_H__

#include "item.h"

#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>

#include <boost/tokenizer.hpp>

using muduo::string;

class MemcacheServer;
class Session : QYJ::noncopyable, public std::enable_shared_from_this<Session> {
   public:
    Session(MemcacheServer* owner, const muduo::net::TcpConnectionPtr& conn)
        : owner_(owner),
          conn_(conn),
          state_(kNewCommand),
          protocol_(kAscii),
          noreply_(false),
          policy_(Item::kInvalid),
          bytesToDiscard_(0),
          needle_(Item::makeItem(kLongestKey, 0, 0, 2, 0)),
          bytesRead_(0),
          requestsProcessed_(0) {
        conn_->setMessageCallback(
            std::bind(&Session::onMessage, this, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3));
    }

    ~Session() {
        LOG_INFO << "requests processed: " << requestsProcessed_
                 << "input buffer size: "
                 << conn_->inputBuffer()->internalCapacity()
                 << "output buffer size: "
                 << conn_->outputBuffer()->internalCapacity();
    }

   private:
    enum State {
        kNewCommand,
        kReceiveValue,
        kDiscardValue,
    };
    enum Protocol {
        kAscii,   //字节协议
        kBinary,  //二进制协议
        kAuto,
    };
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf, muduo::Timestamp);
    void onWriteComplete(const muduo::net::TcpConnectionPtr& conn);
    void receiveValue(muduo::net::Buffer* buf);
    void discardValue(muduo::net::Buffer* buf);

    bool processRequest(muduo::StringPiece request);
    void resetRequest();
    void reply(muduo::StringPiece msg);

    struct SpaceSeparator {
        void reset() {}
        template <typename InputIterator, typename Token>
        bool operator()(InputIterator& next, InputIterator end, Token& tok);
    };

    using Tokenizer =
        boost::tokenizer<SpaceSeparator, const char*, muduo::StringPiece>;
    struct Reader;
    bool doUpdate(Tokenizer::iterator& beg, Tokenizer::iterator end);
    void doDelete(Tokenizer::iterator& beg, Tokenizer::iterator end);

    MemcacheServer* owner_;
    muduo::net::TcpConnectionPtr conn_;
    State state_;
    Protocol protocol_;

    string command_;
    bool noreply_;
    Item::UpdatePolicy policy_;
    ItemPtr currItem_;
    size_t bytesToDiscard_;

    ItemPtr needle_;
    muduo::net::Buffer outputBuf_;

    size_t bytesRead_;
    size_t requestsProcessed_;

    static string kLongestKey;
};

using SessionPtr = std::shared_ptr<Session>;

#endif