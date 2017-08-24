# -*- mode: ruby -*-
# vi: set ft=ruby :

# Layer 1 - set environment variables
$layer1 = <<SCRIPT
export LLVM_VERSION=3.4
export KLEE_BUILD_DIR=/home/vagrant/klee/build
export KLEE_UCLIBC_SOURCE_DIR=/home/vagrant/klee-uclibc
export HOME=/home/vagrant
SCRIPT

# Layer 2 - Install LLVM toolchain
$layer2 = <<SCRIPT
apt-get update && \
apt-get install -y \
clang-3.4 \
llvm-3.4 \
llvm-3.4-dev \
llvm-3.4-runtime
SCRIPT

# TODO: add 'dwarves'
# Layer 3 - KLEE deps and other packages
$layer3 = <<SCRIPT
apt-get update && \
apt-get install -y \
build-essential \
curl \
libcap-dev \
git \
cmake \
libncurses5-dev \
python-minimal \
python-pip \
unzip \
zlib1g-dev \
flex \
bison \
python3-tk \
python3-pip \
pkg-config \
libfreetype6-dev \
vim \
dos2unix
SCRIPT

# Layer 4 - Install minisat
$layer4 = <<SCRIPT
git clone https://github.com/stp/minisat.git
mkdir -p minisat/build
cd minisat/build
cmake -DSTATIC_BINARIES=ON -DCMAKE_INSTALL_PREFIX=/usr/local ../
make
sudo make install
SCRIPT

# Layer 5 - Install stp
$layer5 = <<SCRIPT
cd ../../
git clone https://github.com/stp/stp.git
cd stp
git checkout tags/2.1.2
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_PYTHON_INTERFACE:BOOL=OFF ../
make
sudo make install
cd ../
ulimit -s unlimited
cd ../
SCRIPT

# Layer 6 - Symbolic links
$layer6 = <<SCRIPT
ln -s /usr/bin/llvm-config-3.4 /usr/local/bin/llvm-config
SCRIPT

# Layer 7 - Install klee and klee-uclibc
$layer7 = <<SCRIPT
git clone https://github.com/klee/klee-uclibc.git
cd klee-uclibc
./configure --make-llvm-lib
make -j2
cd ..
git clone https://github.com/klee/klee.git
cd klee
mkdir build
cd build
cmake -DENABLE_SOLVER_STP=ON \
 -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-3.4 \
 -DENABLE_UNIT_TESTS=OFF \
 -DENABLE_POSIX_RUNTIME=ON \
 -DENABLE_KLEE_UCLIBC=ON \
 -DKLEE_UCLIBC_PATH=/home/vagrant/klee-uclibc \
 -DENABLE_SYSTEM_TESTS=OFF ../
make
cd ../../
SCRIPT

# Layer 8 - More symlinks and environment variables
$layer8 = <<SCRIPT
touch /home/vagrant/.bashrc
(echo 'export PATH=$PATH:'/home/vagrant/klee/build'/bin' >> /home/vagrant/.bashrc)
export LLVM_COMPILER=clang
(for f in /home/vagrant/klee/build/bin/* ; do ln -s ${f} /usr/local/bin/`basename ${f}`; done)
SCRIPT

# Layer 9 - Install additional python packages
$layer9 = <<SCRIPT
pip install wllvm
pip3 install GitPython
pip3 install numpy
pip3 install matplotlib
SCRIPT

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://vagrantcloud.com/search.
  config.vm.box = "ubuntu/trusty64"
  config.ssh.forward_agent = true
  config.ssh.forward_x11 = true
  config.vm.provision "shell", inline: $layer1
  config.vm.provision "shell", inline: $layer2
  config.vm.provision "shell", inline: $layer3
  config.vm.provision "shell", inline: $layer4
  config.vm.provision "shell", inline: $layer5
  config.vm.provision "shell", inline: $layer6

  # TODO: Check whether '/home/urop' can be changed to /home/urop
  config.vm.synced_folder "./", "/home/vagrant/UROP"

  config.vm.provision "shell", inline: $layer7
  config.vm.provision "shell", inline: $layer8
  config.vm.provision "shell", inline: $layer9
  config.vm.provision "shell", inline: "cd /home/urop"
  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # NOTE: This will enable public access to the opened port
  # config.vm.network "forwarded_port", guest: 80, host: 8080

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine and only allow access
  # via 127.0.0.1 to disable public access
  # config.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"

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
  # config.vm.synced_folder "../UROP", "/home/vagrant/"

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

  # Enable provisioning with a shell script. Additional provisioners such as
  # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
  # documentation for more information about their specific syntax and use.
  # config.vm.provision "shell", inline: <<-SHELL
  #   apt-get update
  #   apt-get install -y apache2
  # SHELL
end
