#include <iostream>
#include "tensor.h"
#include "proxy.h"
#include "UTensor.h"

int main() {
    UTensor::UTensor<int> t1(2,3);
    UTensor::UTensor<int> t2(3,2);

    //tensor.set(10,1,1);

    proxy_label_tensor<int, char> sp_t1(t1, {'n', 'm'});




    //t1.slicing(2,2);
    return 0;
}
