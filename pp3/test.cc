#include "hashtable.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {

	string *s1 = new string("found");
	Hashtable<string *> *h = new Hashtable<string *>();

	h->Enter(argv[1], s1);
	if(h->Lookup(argv[2])) {
		cout << h->Lookup(argv[2]) << endl;
	} else {
		cout << "Not found" << endl;
	}

}
