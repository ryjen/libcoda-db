FROM ubuntu:14.04

RUN apt-get -y install software-properties-common && add-apt-repository ppa:george-edison55/cmake-3.x && apt-get update
RUN apt-get -y install build-essential cmake cmake-data valgrind lcov libmysqlclient-dev libsqlite3-dev libpq-dev mysql-server postgresql-9.3 postgresql-client-9.3 postgresql-contrib-9.3

USER postgres
RUN service postgresql start \
    && psql --command "CREATE USER pguser WITH SUPERUSER PASSWORD 'pguser';" \
    && createdb -O pguser test

USER root

WORKDIR /etc/postgresql/9.3/main

RUN echo "host all  all    0.0.0.0/0  md5" >> pg_hba.conf
RUN echo "listen_addresses='*'" >> postgresql.conf

RUN service mysql start \
    && mysql -e "create database test"

RUN mkdir -p /usr/src/arg3db/build

COPY . /usr/src/arg3db

WORKDIR /usr/src/arg3db/build

RUN cmake -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON -DMEMORY_CHECK=ON ..

RUN make && make test ARGS=-V
