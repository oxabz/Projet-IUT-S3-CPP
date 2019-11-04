#include <iostream>
#include <string>

void exchange(int &param1, int &param2){
	int test1;
	test1 = param1;
	param1 = param2;
	param2 = test1;
}

int main() {
	int test1,test2;
	test1 = 1;
	test2 = 2;
	exchange(test1,test2);
	return 0;
}