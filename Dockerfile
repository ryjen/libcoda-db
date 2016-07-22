
FROM ubuntu

RUN apt-get update

RUN apt-get -y install build-essential g++ gdb cmake valgrind lcov libmysqlclient-dev libsqlite3-dev libpq-dev postgresql-server-dev-all wget libsoci-dev libpoco-dev

# dockerize for docker-compose
# used to wait for databases to be ready
RUN wget https://github.com/jwilder/dockerize/releases/download/v0.2.0/dockerize-linux-amd64-v0.2.0.tar.gz

RUN tar -C /usr/local/bin -xzvf dockerize-linux-amd64-v0.2.0.tar.gz

ENV BUILD_SRC /usr/src

WORKDIR ${BUILD_SRC}

RUN mkdir -p build cmake libs src tests

COPY cmake/ cmake/
COPY libs/ libs/
COPY src/ src/
COPY tests/ tests/
COPY CMakeLists.txt .

WORKDIR ${BUILD_SRC}/build

RUN cmake -DCMAKE_BUILD_TYPE=Release -DMEMORY_CHECK=ON -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/9.5/server ..

RUN make

CMD "make", "test", "ARGS=-V"
