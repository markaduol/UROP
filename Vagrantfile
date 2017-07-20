# -*- mode: ruby -*-
# vi: set ft=ruby :

$stp_script = <<SCRIPT
apt-get install -y zlib1g-dev flex bison
git clone https://github.com/stp/minisat.git
cd minisat
mkdir build
cd build
cmake -DSTATIC_BINARIES=ON -DCMAKE_INSTALL_PREFIX=/usr/local ../
make
sudo make install
cd ../../
git clone https://github.com/stp/stp.git
cd stp
git checkout tags/2.1.2
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_PYTHON_INTERFACE:BOOL=OFF ..
make
sudo make install
cd ..
ulimit -s unlimited
cd ..
SCRIPT

$klee_script = <<SCRIPT
git clone https://github.com/klee/klee.git
mkdir build
cd build
cmake -DENABLE_SOLVER_STP=ON -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-3.9 -DENABLE_UNIT_TESTS=OFF -DENABLE_SYSTEM_TESTS=OFF ../
make
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
  # Install LLVM and Clang
  config.vm.provision "shell", inline: "wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -"
  config.vm.provision "shell", inline: "sudo apt-add-repository \"deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main\""
  config.vm.provision "shell", inline: "sudo apt-get update"
  config.vm.provision "shell", inline: "sudo apt-get install -y clang-3.9 clang-3.9-doc libclang-common-3.9-dev libclang-3.9-dev libclang1-3.9 libclang1-3.9-dbg libllvm-3.9-ocaml-dev libllvm3.9 libllvm3.9-dbg lldb-3.9 llvm-3.9 llvm-3.9-dev llvm-3.9-doc llvm-3.9-examples llvm-3.9-runtime clang-format-3.9 python-clang-3.9 libfuzzer-3.9-dev"
  # Install klee dependencies
  config.vm.provision "shell", inline: "sudo apt-get install -y build-essential curl libcap-dev git cmake libncurses5-dev python-minimal python-pip unzip"
  # Install STP
  config.vm.provision "shell", inline: $stp_script
  # Install klee
  config.vm.provision "shell", inline: $klee_script 
  # Setup project
  config.vm.provision "shell", inline: "cd /data/UROP/lib/re2"
  config.vm.provision "shell", inline: "CXX=clang++ make"
  config.vm.provision "shell", inline: "cd /data/UROP/lib/re2-2017-06-01"
  config.vm.provision "shell", inline: "CXX=clang++ make"
  config.vm.provision "shell", inline: "cd /data/UROP"
  config.vm.provision "shell", inline: "make"

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
  config.vm.synced_folder "../data/UROP", "/vagrant_data/UROP"

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
