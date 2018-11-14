
FROM ryjen/cpp-coveralls

ARG CMAKE_DEFINES

RUN apk update

RUN apk add mariadb-dev \
    mariadb-connector-c-dev \
    sqlite-dev \
    libpq \
    postgresql-dev
    

ENV BUILD_SRC /usr/src

WORKDIR ${BUILD_SRC}

COPY . ${BUILD_SRC}

RUN mkdir -p ${BUILD_SRC}/build

WORKDIR ${BUILD_SRC}/build

RUN cmake ${CMAKE_DEFINES} -DPostgreSQL_TYPE_INCLUDE_DIR="$(pg_config --includedir-server)" ..

RUN make

CMD "make", "test", "ARGS=-V"
