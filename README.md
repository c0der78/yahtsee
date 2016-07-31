terminal-yahtsee
================

[![Join the chat at https://gitter.im/ryjen/yahtsee](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/ryjen/yahtsee?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

a multiplayer game of yahtzee on the terminal

#### required libraries

- [libcaca](http://caca.zoy.org/wiki/libcaca) (brew install libcaca / apt-get install libcaca-dev) For the ascii display driver

- [libryjen](http://github.com/ryjen/libryjen.git) (cmake) for networking, json, etc

- [libarchive](http://www.libarchive.org) (brew install libarchive / apt-get install libarchive-dev) for reading assets

#### optional libraries

- [libminiupnpc](http://miniupnp.free.fr) (brew install miniupnpc / apt-get install libminiupnpc-dev) for opening network ports on routers

#### build / install for homebrew

- mkdir build; cd build
- cmake $(cd ..;brew diy --version=0.1.0) ..
- make
- make install
- brew link yahtsee

#### TODO
- Refactor the event system so everything goes through that.
- ~~build [connection service](http://arg3connect.herokuapp.com) to register/list available online games~~
- ~~implement turn based game play~~
- ~~implement server mode w/ protocol~~
- ~~implement client mode w/ protocol~~
- ~~implement UPnP to open port on routers automatically~~
- test, test, test
- make homepage, better screenshots
- roll yahtsee animation?, win game animation?

#### Sample

![Yahtsee Example](yahtsee.gif?raw=true "Yahtsee Example")

