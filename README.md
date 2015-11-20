terminal-yahtsee
================

[![Join the chat at https://gitter.im/c0der78/yahtsee](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/c0der78/yahtsee?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

a multiplayer game of yahtzee on the terminal

#### required libraries

- [libcaca](http://caca.zoy.org/wiki/libcaca) (brew install libcaca / apt-get install libcaca-dev) For the ascii display driver

- [libarg3](http://github.com/c0der78/libarg3.git) (./configure; make install) for networking, json, etc

- [libarchive](http://www.libarchive.org) (brew install libarchive / apt-get install libarchive-dev) for reading assets

#### optional libraries

- [libminiupnpc](http://miniupnp.free.fr) (brew install miniupnpc / apt-get install libminiupnpc-dev) for opening network ports on routers

#### TODO
- Refactor the event system so everything goes through that.
- ~~build [arg3connect](http://arg3connect.herokuapp.com) to register/list available online games~~
- ~~implement turn based game play~~
- ~~implement server mode w/ protocol~~
- ~~implement client mode w/ protocol~~
- ~~implement UPnP to open port on routers automatically~~
- test, test, test
- make homepage, better screenshots
- roll yahtsee animation?, win game animation?

#### Screenshots

![Yahtsee 1](yahtsee1.png?raw=true "Yahtsee 1")

![Yahtsee 2](yahtsee2.png?raw=true "Yahtsee 2")

![Yahtsee 3](yahtsee3.png?raw=true "Yahtsee 3")
