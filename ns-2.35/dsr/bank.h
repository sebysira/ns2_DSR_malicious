/*
 * Each node will have a Bank object which keeps track
 * of the chip count of that node neighbours, using a 
 * mapping of nsaddr_t to class BankEntry.
 *
 */

#ifndef BANK_H
#define BANK_H

#include <map>
#include <list>
#include <config.h> //included for nsaddr_t

#define DEBUG_MALICIOUS false

class BankEntry
{
 private:

  int sendedPackets;
  int confirmedPackets;

 public:
  BankEntry();
  ~BankEntry();
  // construct a new Entry, with $ init value init,
  // rating value initRat
  BankEntry(double init, int initRat);
  // return the chip amount of this bank entry

  // increment the # of packets I have forwarded for this node
  void incSendedPackets();
  // increment the # of packets this node has forwarded for me
  void incConfirmedPackets();
  // get the # of packets I have forwarded for this node
  int getSendedPackets();
  // get the # of packets this node has forwarded for me
  int getConfirmedPackets();

};

class Bank
{
 private:
  friend class Monitor;
  friend class DSRAgent;

  // a mapping of node addresses to BankEntries
  map<nsaddr_t, BankEntry> bankTable;
  // returns a pointer to a bank entry for a given
  // address. If that address is not in this bank,
  // return null
  BankEntry* getBankEntry(nsaddr_t address);

 public:
  Bank();
  ~Bank();
  // add a new entry to the bank. return true if
  // entry is added, false if it already exists
  bool addNewEntry(nsaddr_t address);
  // remove an entry from the bank. return true if
  // it has been removed successfully, false if it
  // has not or it does not exist
  bool removeEntry(nsaddr_t address);
  // check if an entry already exists in this bank.
  // return true if it does, false if it does not
  bool contains(nsaddr_t address);
  // print bank to file
  void printBank(char * net_id);
};
  
#endif
