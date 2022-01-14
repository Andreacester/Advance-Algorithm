#include <iostream>
#include "tensor.h"

int main() {
    Tensor<int,2>  tensor(10, 10);
    int n=0;
    //tensor.set(10,1,1);

    for(int r= 0; r < 10; ++r)
        for(int c = 0; c < 10; ++c){
            tensor.set(n, r, c);
            n++;
        }


    tensor.printTensor("ciao");
    return 0;
}