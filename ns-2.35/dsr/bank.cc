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

  int BankEntry::getPacchettiInviati(){
    return pacchettiInviati;
  }
  int BankEntry::getPacchettiConfermati(){
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

void Bank::stampa(char * net_id) {
    FILE * result;
    result = fopen("result.txt", "a");
    if(result==NULL) {
      cout << "Error opening file." << endl;
      return;
    }

    FILE * valuation;
    valuation = fopen("valuation.txt", "a");
    if(valuation==NULL) {
      cout << "Error opening file." << endl;
      return;
    }

    fprintf(result, "***********************\n");
    fprintf(result, "Result for node #%s\n\n", net_id);

    for( map<nsaddr_t, BankEntry>::iterator it = bankTable.begin(); it != bankTable.end(); ++it) {

        BankEntry entry = it->second;
        fprintf(result, "Node #%d: Packets sended = %d - Packets confirmed = %d\n", it->first, entry.getPacchettiInviati(), entry.getPacchettiConfermati());
        //float percentage = (entry.getPacchettiConfermati() * 100.0) / entry.getPacchettiInviati();
        fprintf(valuation, "%d %d %d\n", it->first, entry.getPacchettiInviati(), entry.getPacchettiConfermati());
    }
    fprintf(result, "***********************\n\n");
    fclose(result);
    fclose(valuation);

}





