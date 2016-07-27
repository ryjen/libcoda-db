
FROM ubuntu

ARG CMAKE_DEFINES

RUN apt-get update

RUN apt-get -y install build-essential g++ clang gdb cmake valgrind lcov libmysqlclient-dev libsqlite3-dev libpq-dev postgresql-server-dev-all wget libsoci-dev libpoco-dev ruby git

# dockerize for docker-compose
# used to wait for databases to be ready
RUN wget https://github.com/jwilder/dockerize/releases/download/v0.2.0/dockerize-linux-amd64-v0.2.0.tar.gz

RUN tar -C /usr/local/bin -xzvf dockerize-linux-amd64-v0.2.0.tar.gz

RUN gem install coveralls-lcov

ENV BUILD_SRC /usr/src

WORKDIR ${BUILD_SRC}

COPY . ${BUILD_SRC}

RUN mkdir -p ${BUILD_SRC}/build

WORKDIR ${BUILD_SRC}/build

RUN cmake ${CMAKE_DEFINES} -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/9.5/server ..

RUN make

CMD "make", "test", "ARGS=-V"
