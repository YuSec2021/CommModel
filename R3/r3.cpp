#include "Comm/comm.h"
#include <stdio.h>

int main() {
	Test t = { 0 };
	t.x = 100;
	t.y = 200;
	DeviceComm(TEST, &t, 4, NULL, NULL);
	printf("x:%x y:%x\r\n", t.x, t.y);
	return 0;
}