yahtsee
=======

[![Join the chat at https://gitter.im/ryjen/yahtsee](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/ryjen/yahtsee?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

a multiplayer game of yahtzee.  Trying to support two UI's - Terminal based (ncurses) and Graphical (imgui).


#### building

This project now builds with [prep](https://github.com/ryjen/prep).  

If you install prep you should get a cmake module, otherwise just run `prep`.


#### TODO/IDEAS

[ ] Refactor the event system so everything goes through that.
[ ] build connection service to register/list available online games
[x] implement turn based game play
[x] implement server mode w/ protocol
[x] implement client mode w/ protocol
[x] implement UPnP to open port on routers automatically
[ ] test, test, test
[ ] make homepage, better screenshots
[ ] roll yahtsee animation?, win game animation?

#### Sample

![Yahtsee Example](yahtsee.gif?raw=true "Yahtsee Example")

