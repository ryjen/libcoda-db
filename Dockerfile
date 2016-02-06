# Need to link this container with mysql and postgres, both with 'test' databases
# docker run --link=mysql:mysqltest --link=postgres:postgrestest arg3db
#

FROM ubuntu:14.04

RUN apt-get -y install software-properties-common && add-apt-repository ppa:george-edison55/cmake-3.x && apt-get update

RUN apt-get -y install build-essential cmake cmake-data valgrind lcov libmysqlclient-dev libsqlite3-dev libpq-dev

RUN mkdir -p /usr/src/arg3db/build

COPY . /usr/src/arg3db

WORKDIR /usr/src/arg3db/build

RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON -DMEMORY_CHECK=ON ..

RUN make

CMD ["/bin/bash", "/usr/src/arg3db/build/tests/arg3db-test"]
