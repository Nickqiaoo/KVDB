#ifndef SERVER_H__
#define SERVER_H__

#include "item.h"
#include"session.h"

#include <muduo/base/Mutex.h>
#include <muduo/net/TcpServer.h>

#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class MemcacheServer : noncopyable {
   public:
    struct Options {
        Options();
        uint16_t tcpport;
        uint16_t udpport;
        uint16_t gperfport;
        int threads;
    };
    MemcacheServer(muduo::net::EventLoop* loop, const Options&);
    ~MemcacheServer();

    void setThreadNum(int threads) { server_.setThreadNum(threads);}
    void start();
    void stop();

    time_t startTime() const { return startTime_; }

    bool storeItem(const ItemPtr& item, Item::UpdatePolicy policy, bool* exits);    //存
    ConstItemPtr getItem(const ConstItemPtr& key) const;    //取
    bool deleteItem(const ConstItemPtr& key);   //删

   private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    struct Stats;
    muduo::net::EventLoop* loop_;
    Options options_;
    const time_t startTime_;
    mutable muduo::MutexLock mutex_;
    std::unordered_map<string, SessionPtr> sessions_;   //一个连接对应一个Session

    struct Hash {
        size_t operator()(const ConstItemPtr& x) const { return x->hash(); }
    };

    struct Equal {
        bool operator()(const ConstItemPtr& x, const ConstItemPtr& y) const {
            return x->hash() == y->hash();
        }
    };
    using ItemMap = std::unordered_set<ConstItemPtr, Hash, Equal>;

    struct MapWithLock {
        ItemMap items;
        mutable muduo::MutexLock mutex;
    };

    const static int kShards = 4096;
    std::array<MapWithLock, kShards> shards_;

    muduo::net::TcpServer server_;
    std::unique_ptr<Stats> stats_;
};

#endif