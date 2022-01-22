#include <iostream>
#include "tensor.h"

int main() {
    Tensor::Tensor<int,2> t23(2, 3);
    int n=0;
    //tensor.set(10,1,1);




    t23.slicing(2,2);
    return 0;
}
