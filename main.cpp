#include <iostream>

#include "tensor.h"
#include "proxy.h"

int main() {

    Tensor::Tensor<int,2> t1(2,3);


    Tensor::Tensor<int,2> t23(2, 3);
    Tensor::Tensor<int,2> t32(3, 2);

    /*int count=0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            t1.set(count,i,j);
            count++;
        }

    }
    t1.printTensor("ciao");

    proxy_label_tensor<int,2,char> tl(t1,{'i','j'});
*/

    int count=0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            t23.set(count,i,j);
            count++;
        }

    }
    count=0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            t32.set(count,i,j);
            count++;
        }

    }

    t23.printTensor("ciao");
    t32.printTensor("nome");

    proxy_label_tensor<int,2, char> sp_t1(t23, {'n', 'm'});
    proxy_label_tensor<int,2, char> sp_t2(t32, {'m', 'p'});


    //proxy_op_tensor<int,2,char> r(sp_t2 * sp_t1,{'m','m'});

    //Tensor::Tensor<int,2> result = static_cast<Tensor::Tensor<int,2>>(sp_t1);
    //Tensor::Tensor<int,2> result = (sp_t2 * sp_t1);

    proxy_label_tensor<int,2, char> c(sp_t1+sp_t2);


    return 0;
}
