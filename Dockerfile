
FROM ryjen/cpp-coveralls

ARG CMAKE_DEFINES

ENV POSTGRES_VERS 9.3

RUN apt-get -y install libmysqlclient-dev libsqlite3-dev libpq-dev postgresql-server-dev-${POSTGRES_VERS} libboost-dev

ENV BUILD_SRC /usr/src

WORKDIR ${BUILD_SRC}

COPY . ${BUILD_SRC}

RUN mkdir -p ${BUILD_SRC}/build

WORKDIR ${BUILD_SRC}/build

RUN cmake ${CMAKE_DEFINES} -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/${POSTGRES_VERS}/server ..

RUN make

CMD "make", "test", "ARGS=-V"
