
FROM ryjen/cpp-coveralls

ARG CMAKE_DEFINES

RUN apt-get -y install libmysqlclient-dev libsqlite3-dev libpq-dev postgresql-server-dev-9.3 libboost-dev

ENV BUILD_SRC /usr/src

WORKDIR ${BUILD_SRC}

COPY . ${BUILD_SRC}

RUN mkdir -p ${BUILD_SRC}/build

WORKDIR ${BUILD_SRC}/build

RUN cmake ${CMAKE_DEFINES} -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/9.3/server ..

RUN make

CMD "make", "test", "ARGS=-V"
