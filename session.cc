#include"session.h"
#include"server.h"

using namespace muduo;
using namespace muduo::net;

static bool isBinaryProtocol(uint8_t firstByte){
    return firstByte==0x80;
}

const int kLongestKeySize =250;
string Session::kLongestKey(kLongestKey,'x');

template<typename InputIterator,typename Token>
bool Session::SpaceSeparator::operator()(InputIterator& next,InputIterator end,Token& tok){
    
}
