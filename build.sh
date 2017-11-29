g++ -o memcached-debug  serve.cc server.cc session.cc item.cc  -std=c++14 \
-L /home/qyj/下载/build/release-install-cpp11/lib  \
-lmuduo_inspect -lmuduo_http -lmuduo_net -lmuduo_base -lpthread  -lboost_program_options -g