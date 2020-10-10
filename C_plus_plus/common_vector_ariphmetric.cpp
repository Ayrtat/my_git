#include <cassert>
#include <iostream>
#include <vector>

template<typename T, typename Cont = std::vector<T> >
class MyVector {
    Cont cont;

public:
    // MyVector with initial size
    MyVector(const std::size_t n) : cont(n) {}

    // MyVector with initial size and value
    MyVector(const std::size_t n, const double initialValue) : cont(n, initialValue) {}

    // Constructor for underlying container
    MyVector(const Cont& other) : cont(other) {}

    // assignment operator for MyVector of different type
    template<typename T2, typename R2>
    MyVector& operator=(const MyVector<T2, R2>& other) {
        assert(size() == other.size());
        for (std::size_t i = 0; i < cont.size(); ++i) cont[i] = other[i];
        return *this;
    }

    // size of underlying container
    std::size_t size() const {
        return cont.size();
    }

    // index operators
    T operator[](const std::size_t i) const {
        return cont[i];
    }

    T& operator[](const std::size_t i) {
        return cont[i];
    }

    // returns the underlying data
    const Cont& data() const {
        return cont;
    }

    Cont& data() {
        return cont;
    }
};

template<class BinOperation, typename T, typename Op1, typename Op2>
class MyVectorOp {
    const Op1& op1;
    const Op2& op2;

    BinOperation operation{};
public:
    MyVectorOp(const Op1& a, const Op2& b) : op1(a), op2(b) {}

    T operator[](const std::size_t i) const {
        return operation(op1[i], op2[i]);
    }

    std::size_t size() const {
        return op1.size();
    }
};

template<typename T, typename Op1, typename Op2>
class MyVectorAdd {
    const Op1& op1;
    const Op2& op2;

public:
    MyVectorAdd(const Op1& a, const Op2& b) : op1(a), op2(b) {}

    T operator[](const std::size_t i) const {
        return op1[i] + op2[i];
    }

    std::size_t size() const {
        return op1.size();
    }
};

// elementwise MyVector * MyVector
template< typename T, typename Op1, typename Op2 >
class MyVectorMul {
    const Op1& op1;
    const Op2& op2;

public:
    MyVectorMul(const Op1& a, const Op2& b) : op1(a), op2(b) {}

    T operator[](const std::size_t i) const {
        return op1[i] * op2[i];
    }

    std::size_t size() const {
        return op1.size();
    }
};

template<typename T, typename Container1, typename Container2>
auto
operator+ (const MyVector<T, Container1>& a, const MyVector<T, Container2>& b) {
    return  MyVector<T, MyVectorOp<std::plus<T>, T, Container1, Container2>>(MyVectorOp<std::plus<T>, T, Container1, Container2>(a.data(), b.data()));
}

template<typename T, typename Container1, typename Container2>
auto
operator* (const MyVector<T, Container1>& a, const MyVector<T, Container2>& b) {
    return MyVector<T, MyVectorOp<std::multiplies<T>, T, Container1, Container2>>(MyVectorOp<std::multiplies<T>, T, Container1, Container2>(a.data(), b.data()));
}

// function template for < operator
template<typename T>
std::ostream& operator<<(std::ostream& os, const MyVector<T>& cont) {
    std::cout << std::endl;
    for (int i = 0; i < cont.size(); ++i) {
        os << cont[i] << ' ';
    }
    os << std::endl;
    return os;
}

int main() {
    
      MyVector<double> x(10,5.4);
      MyVector<double> y(10,10.3);

      MyVector<double> result(10);

      result = x + x + y * y;

      std::cout << result << std::endl;
      
}
