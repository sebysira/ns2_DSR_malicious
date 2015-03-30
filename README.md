DSR with malicious nodes
==========

Università di Catania -
CdL Informatica Magistrale -
P2P and wireless networks A.A 2014/15

Autors:
- [Cantarella Danilo](https://github.com/Flyer-90) (http://cantarelladanilo.com);
- [Siragusa Sebastiano](https://github.com/sebysira) (http://sebastianosiragusa.altervista.org);

Introduction
============

This is a project that allows you to add malicious nodes in NS2 using DSR protocol.
The project was written in TCL and C++ and used NS2 simulator.
It's released under the GNU GPL v3 License.

Install
=======

- Copy ns-2.35 folder into your ns-allinone-2.35 directory
- Include bank.o and monitor.o in your Makefile.in 
- Run ./configure in your ns-allinone-2.35 directory 
- Run make depend in ns-allinone-2.35
- Run make
- Run sudo make install
- Run dsr_malicious.tcl with ns command

