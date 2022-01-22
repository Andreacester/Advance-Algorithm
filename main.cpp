#include <iostream>
#include "tensor.h"
#include "proxy.h"

int main() {
    Tensor::Tensor<int> t1(2);
    Tensor::Tensor<int> t2(3);

    //tensor.set(10,1,1);
    proxy_label_tensor<int, char> sp_t1(t1, {'n', 'm'});
    proxy_label_tensor<int, char> sp_t2(t2, {'m', 'p'});



    //t1.slicing(2,2);
    return 0;
}
