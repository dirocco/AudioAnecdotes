unsigned char value, *ptr;
int x, size;

   struct {
      char  a;
      short b;
   } testStruct;

   testStruct.a= 0x12;
   testStruct.b= 0x3456;
   size= sizeof(testStruct);
   ptr= (char*)&testStruct;

   printf("size:%d\n", size);
   for(x= 0; x<size; x++) {
      value = (unsigned char)*ptr++; 
      printf("%02X\n", (unsigned int)value;
   }
