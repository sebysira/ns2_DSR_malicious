#include "bank.h"
#include <iostream> // for print debugging

BankEntry::BankEntry() {}

BankEntry::~BankEntry() {}

BankEntry::BankEntry(double init, int initRat) {
  myForwardingCount = 0;
  theirForwardingCount = 0;

  pacchettiInviati = 0;
  pacchettiConfermati = 0;
}

void BankEntry::incMyForwardingCount() {
  myForwardingCount++;
}

void BankEntry::incTheirForwardingCount() {
  theirForwardingCount++;
}

double BankEntry::getMyForwardingCount() {
  return myForwardingCount;
}

double BankEntry::getTheirForwardingCount() {
  return theirForwardingCount;
}

void BankEntry::incPacchettiInviati() {
  pacchettiInviati++;
}
  void BankEntry::incPacchettiConfermati() {
    pacchettiConfermati++;
  }

  double BankEntry::getPacchettiInviati(){
    return pacchettiInviati;
  }
  double BankEntry::getPacchettiConfermati(){
    return pacchettiConfermati;
  }
  


Bank::Bank() {}

Bank::~Bank() { delete &bankTable; }

BankEntry* Bank::getBankEntry(nsaddr_t address) {
  if(contains(address)) {
    return &bankTable[address];
  }
  return NULL;
}

bool Bank::addNewEntry(nsaddr_t address) {
  if(!contains(address)) {
    bankTable[address] = BankEntry(0, 0);
    return true;
  }
  return false;
}

bool Bank::removeEntry(nsaddr_t address) {
  if(contains(address)) {
    bankTable.erase(address); 
    return true;
  }
  return false;
}

bool Bank::contains(nsaddr_t address) {
  if(bankTable.count(address) != 0) {
    return true;
  }
  return false;
}

bool Bank::stampa(char * net_id) {

cout<< "***********************" << endl;
cout<< "Bank del nodo # " << net_id << endl;
for( map<nsaddr_t, BankEntry>::iterator it = bankTable.begin(); it != bankTable.end(); ++it) {

    BankEntry entry = it->second;
    cout << "Nodo # " <<  it->first << " Pacchetti inviati " << entry.getPacchettiInviati() << " di cui confermati "<< entry.getPacchettiConfermati() << "\n";
}
cout<< "***********************" << endl << endl;


  
  return 0.0;
}





