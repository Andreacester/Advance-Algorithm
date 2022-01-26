#include <iostream>

#include "tensor.h"
#include "proxy.h"

int main() {

    Tensor::Tensor<int,2> t1(2,3);

    int count=0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            t1.set(count,i,j);
            count++;
        }

    }
    t1.printTensor("ciao");

    proxy_label_tensor<int,2,char> tl(t1,{'i','j'});



    //t1.slicing(2,2);
    return 0;
}
