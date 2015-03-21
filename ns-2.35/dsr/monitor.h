/*
 * Each node will have a monitor object which will 
 * cache sent packets and then listen to any packets
 * which are sent by other nodes in its range, and compare
 * those packets to the sent packets to see if its neighbouring
 * nodes are forwarding for this node, and hence behaving correctly.
 *
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <map>
#include <list>
#include <packet.h>
#include <trace.h>
#include <mac.h>
#include "srpacket.h"
#include "bank.h"
#include "routecache.h"

typedef double Time; // can remove this if we ever #include path.h
                     // since it is defined there also

class SentPacket
{
 private:
  Time time_sent;
  Packet packet;
  Trace* trace;

 public:
  SentPacket();
  ~SentPacket();
  SentPacket(Packet* pack, Time time, Trace* t);
  // get the time the packet was sent
  Time getTimeSent();
  // get the packet's sequence number
  int getSeqNo();
  // get the actual packet
  Packet* getPacket();
};  


class Monitor
{
 private:
  // a mapping of node addresses to lists of sent packets.
  // note: this node address should always be the
  // address of the net hop of the packet. Eg: if
  // A sends to C through B, then when A caches the
  // packet, this address should be the address of
  // B, since B will be next to forward.
  map<nsaddr_t, list<SentPacket*>* > sentPacketTable;
  double packet_timeout;
  Bank* bank;
  Trace* trace;
  RouteCache** route_cache;
  // checks the cached packets to see if they have
  // timed out.
  void checkPacketCache(nsaddr_t address);
  // a list of node addresses which this node considers
  // to be faulty
  list<nsaddr_t> faultyList;

 public:
  Monitor();
  ~Monitor();
  Monitor(Trace* t, Bank* bank, RouteCache** r_c);
  // get a given packet's sequence number
  int getSeqNo(Packet packet);
  // will add a packet this node sends to its
  // cache of sent packets
  void addPacketToCache(nsaddr_t address, Packet* packet, Time sending_time);
  // will delete all cached packets sent to given node address
  void flushCache(nsaddr_t address);
  // checks if the packet we have overheard being sent
  // is the same as one in our cache. if it is,
  // we remove it from the cache and return true.
  // if its not the same, return false
  void handleTap(nsaddr_t sender, const Packet* packet, char * net_id);
  // called when a positive event occurs, such as a neighbour
  // forwarding our traffic for us.
  void registerPositiveEvent(nsaddr_t address, char * net_id);
  // called when a negative event occurs, such as a neighbour
  // not forwarding our traffic
  void registerNegativeEvent(nsaddr_t address);
};

class SecondChanceTimer : public TimerHandler {
 public:
  SecondChanceTimer(Monitor* m, nsaddr_t addr) : TimerHandler() { monitor = m; node_address = addr; }
  void expire(Event* e);

 protected:
  Monitor* monitor;
  nsaddr_t node_address;
};

#endif
