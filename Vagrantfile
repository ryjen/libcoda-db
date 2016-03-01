# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure(2) do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://atlas.hashicorp.com/search.
  config.vm.box = "ubuntu/trusty64"

  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # config.vm.network "forwarded_port", guest: 80, host: 8080

  # Create a private network, which allows host-only access to the machine
  # using a specific IP.
  # config.vm.network "private_network", ip: "192.168.33.10"

  # Create a public network, which generally matched to bridged network.
  # Bridged networks make the machine appear as another physical device on
  # your network.
  # config.vm.network "public_network"

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  # config.vm.synced_folder "../data", "/vagrant_data"

  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  # Example for VirtualBox:
  #
  # config.vm.provider "virtualbox" do |vb|
  #   # Display the VirtualBox GUI when booting the machine
  #   vb.gui = true
  #
  #   # Customize the amount of memory on the VM:
  #   vb.memory = "1024"
  # end
  #
  # View the documentation for the provider you are using for more
  # information on available options.

  # Define a Vagrant Push strategy for pushing to Atlas. Other push strategies
  # such as FTP and Heroku are also available. See the documentation at
  # https://docs.vagrantup.com/v2/push/atlas.html for more information.
  # config.push.define "atlas" do |push|
  #   push.app = "YOUR_ATLAS_USERNAME/YOUR_APPLICATION_NAME"
  # end

  # Enable provisioning with a shell script. Additional provisioners such as
  # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
  # documentation for more information about their specific syntax and use.
   config.vm.provision "shell", inline: <<-SHELL
      sudo apt-get -y install software-properties-common
      sudo add-apt-repository ppa:george-edison55/cmake-3.x
      sudo add-apt-repository ppa:ubuntu-toolchain-r/test
      sudo apt-get update
      export DEBIAN_FRONTEND=noninteractive
      sudo apt-get -q -y install build-essential gcc-5 g++-5 cmake cmake-data valgrind lcov mysql-client-5.5 libmysqlclient-dev libsqlite3-dev libpq-dev postgresql-server-dev-9.3 mysql-server-5.5 postgresql-9.3
      sudo service mysql start
      sudo service postgresql start
      sudo mysql -e "create database IF NOT EXISTS test;"
      sudo -u postgres psql -c "create user vagrant with password 'vagrant';"
      sudo -u postgres psql -c "create database test owner vagrant;"
      sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5      
      mkdir -p /vagrant/.vagrant-build
      cd /vagrant/.vagrant-build
      cmake -DCMAKE_BUILD_TYPE=Debug -DMEMORY_CHECK=ON -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/9.3/server ..
      make
      export POSTGRES_URI="postgres://vagrant:vagrant@localhost/test"
      export MYSQL_URI="mysql://root@localhost/test"
      make test ARGS=-V
   SHELL
end
