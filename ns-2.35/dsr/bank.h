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

class BankEntry
{
 private:
  // the amount of packets we have forwarded for this node
  double myForwardingCount;
  // the amount of packets they have forwarded for us
  double theirForwardingCount;

  double pacchettiInviati;
  double pacchettiConfermati;

 public:
  BankEntry();
  ~BankEntry();
  // construct a new Entry, with $ init value init,
  // rating value initRat
  BankEntry(double init, int initRat);
  // return the chip amount of this bank entry
 
  // increment the # of packets I have forwarded for this node
  void incMyForwardingCount();
  // increment the # of packets this node has forwarded for me
  void incTheirForwardingCount();
  // get the # of packets I have forwarded for this node
  double getMyForwardingCount();
  // get the # of packets this node has forwarded for me
  double getTheirForwardingCount();

  void incPacchettiInviati();
  void incPacchettiConfermati();

  double getPacchettiInviati();
  double getPacchettiConfermati();

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
  // increment a BankEntry's chip count
  // return false if incrementing would put it over
  // the maximum amount.
  bool incChipCount(nsaddr_t address);
  // decrement a BankEntry's chip count. Returns false if
  // decrementing would put it under the minimum amount, zero.
  bool decChipCount(nsaddr_t address);
  // get the chip count of the BankEntry.
  double getChipCount(nsaddr_t address);
  // will increment all entry chip counts with given amount.
  // used by the bank timer class
  void incAllEntries(double amount);

  bool stampa(char * net_id);
};
  
#endif
