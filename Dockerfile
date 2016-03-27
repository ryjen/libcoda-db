
FROM ubuntu:14.04

RUN apt-get -y install software-properties-common && add-apt-repository ppa:george-edison55/cmake-3.x && add-apt-repository ppa:ubuntu-toolchain-r/test && apt-get update

RUN apt-get -y install build-essential gcc-5 g++-5 gdb cmake cmake-data valgrind lcov libmysqlclient-dev libsqlite3-dev libpq-dev postgresql-server-dev-9.3 wget

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5

# dockerize for docker-compose
RUN wget https://github.com/jwilder/dockerize/releases/download/v0.2.0/dockerize-linux-amd64-v0.2.0.tar.gz

RUN tar -C /usr/local/bin -xzvf dockerize-linux-amd64-v0.2.0.tar.gz

ENV BUILD_SRC /usr/src/arg3db

WORKDIR ${BUILD_SRC}

RUN mkdir -p build cmake libs src tests

COPY cmake/ cmake/
COPY libs/ libs/
COPY src/ src/
COPY tests/ tests/
COPY CMakeLists.txt .

WORKDIR ${BUILD_SRC}/build

RUN cmake -DCMAKE_BUILD_TYPE=Debug -DMEMORY_CHECK=ON -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/9.3/server ..

RUN make

CMD "make", "test", "ARGS=-V"
