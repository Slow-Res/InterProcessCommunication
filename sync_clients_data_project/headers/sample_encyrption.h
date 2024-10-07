#include <stddef.h>
#ifndef ENCRYPTION_H

#ifndef ENCRYPTION_KEY 
#define ENCRYPTION_KEY 0xAB
#endif

void encryptData(char *data, size_t length);
void decryptData(char *data, size_t length);



#endif // !ENCRYPTION_H