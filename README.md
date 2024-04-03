## MPD Now Playing

MPD Now Playing is a simple client plasmoid for the Music Player Daemon.

![Screenshot1](img/Screenshot1.png)

### How to install

Make sure you have libmpdclient installed (including development files).

Clone the repo or extract the downloaded archive, change to the extracted folder and type

    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=`kf5-config --prefix` ..
    make
    make install (as root)

Feedback and translations are welcome.

**Enjoy!**

![Screenshot2](img/Screenshot2.png)
