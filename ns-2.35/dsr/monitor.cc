#include "monitor.h"
#include <iostream> // for print debugging

SentPacket::SentPacket() {}

SentPacket::~SentPacket() {}

SentPacket::SentPacket(Packet* pack, Time time, Trace* t) {
  packet = *pack;
  time_sent = time;
  trace = t;
}

Time SentPacket::getTimeSent() { return time_sent; }

Packet* SentPacket::getPacket() { return &packet; }

int SentPacket::getSeqNo() { return trace->get_seqno(&packet); }

Monitor::Monitor() {}

Monitor::~Monitor() {delete &faultyList; delete &sentPacketTable; }

Monitor::Monitor(Trace* t, Bank* b, RouteCache** r_c) { 
  trace = t; 
  bank = b; 
  route_cache = r_c;
}

int Monitor::getSeqNo(Packet packet) { return trace->get_seqno(&packet); }

void Monitor::addPacketToCache(nsaddr_t address, Packet* packet, Time sending_time) {
  // if we dont have a list for this address node
  if(sentPacketTable.count(address) == 0) {
    // create one
    sentPacketTable[address] = new list<SentPacket*>;
  }
  // firstly check any current cached packets from this node
  // to see if they have expired
  checkPacketCache(address);

  // now add the new packet onto the end of this list
  list<SentPacket*>* l = sentPacketTable[address];
  l->push_back(new SentPacket(packet, sending_time, trace));
  //cout << "  [" << Scheduler::instance().clock() << "] Adding packet to cache of node #" << address << ". cache size=" << l->size() << endl << flush;

  
  // if using optimistic scheme, increment the next hop's
  // chipcount at this time (before they actually forward)
  
}


void Monitor::flushCache(nsaddr_t address) {
  list<SentPacket*>* l = sentPacketTable[address];
  delete l;
}

void Monitor::handleTap(nsaddr_t sender_address, const Packet* packet, char * net_id) {
  // check that we have a cache of packets sent to this node,
  // hence we will be expecting them to forward our packets if they
  // require forwarding
  //cout<< "****************medda trasi!*******************"<<endl;
  if(sentPacketTable.count(sender_address) == 0) {
    return;
  } 
  // iterate through the cached list for this node
  list<SentPacket*>* l = sentPacketTable[sender_address];
  list<SentPacket*>::iterator iter = l->begin();

  while(iter != l->end()) {
    SentPacket* sp = *iter;
    iter++;
    // check if the packet sequence numbers are equal
    if(sp->getSeqNo() == getSeqNo(*packet)) { 
      // if yes, then they must have forwarded our packet :-)
      // firstly, we remove it from our cache
      l->remove(sp);
      delete(sp);
      // secondly, if using the pessimistic scheme,
      // we increment their bank balance
	if(!bank->contains(sender_address)) {
	  bank->addNewEntry(sender_address);
	}
	//bank->incChipCount(sender_address);

	//cout << "  [" << Scheduler::instance().clock() << "] node #" << sender_address << " forwarded our packet. Removing it from our cache." << endl << flush;


      // since they forwarded our packet, we register a positive event
      registerPositiveEvent(sender_address, net_id);
    }    

  } 
}

void Monitor::checkPacketCache(nsaddr_t address) {
  double time = Scheduler::instance().clock();
   // iterate through the cached list for this node
  list<SentPacket*>* l = sentPacketTable[address];
  list<SentPacket*>::iterator iter = l->begin();

  while(iter != l->end()) {
    SentPacket* sp = *iter;
    iter++;
    // first check if the cached packet's timeout has expired
    
  }
}

void Monitor::registerPositiveEvent(nsaddr_t address, char * net_id) {
  BankEntry* entry = bank->getBankEntry(address);
  if(entry != NULL) {


    // increase the # of packets they have forwarded for us
    entry->incPacchettiConfermati();
    cout << "[" << Scheduler::instance().clock() << "] node #" << net_id << " incrementa pacchetti confermati per il nodo #" << address << " :: incPacchettiConfermati = " << entry->getPacchettiConfermati() << endl << flush;  
  }
}
