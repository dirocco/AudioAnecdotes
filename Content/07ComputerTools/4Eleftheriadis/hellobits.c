// ****************************************************************
// Example 2: HelloBits described using C++ (a similar construct 
// would be used for Java as well).

class HelloBits {
    unsigned int Bits;
    void get() { 
        Bits=::getuint(8);
    }
};

// ****************************************************************
// Example 3: HelloBits described using Flavor.

class HelloBits {
    unsigned int(8) Bits;
}

// ****************************************************************
