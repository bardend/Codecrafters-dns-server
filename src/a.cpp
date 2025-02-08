#include <iostream>
#include <vector>
#include <memory>

using namespace std;
enum class Tipo {
    Normal,
    Especial
};

class A {
protected:
    std::vector<int> arr;

public:
    A(const std::vector<int>& arr) : arr(arr) {}

    virtual ~A() = default;

    virtual void Initialize() {
        Funcion1();
    }

    virtual void Funcion1() {
        std::cout << "Ejecutando Funcion1\n";
    }

    virtual void Funcion2() {}
};

// Clase derivada para objetos Especiales
class AEspecial : public A {
public:
    AEspecial(const std::vector<int>& arr) : A(arr) {}

    void Initialize() override {
        Funcion1();
        Funcion2();
    }

    void Funcion2() override {
        std::cout << "Ejecutando Funcion2 (Especial)\n";
    }
};

// Fábrica de objetos
class AFactory {
public:
    static std::unique_ptr<A> Create(const std::vector<int>& arr, Tipo tipo) {
        std::unique_ptr<A> obj;
        if (tipo == Tipo::Especial) {
            obj = std::make_unique<AEspecial>(arr);
        } else {
            obj = std::make_unique<A>(arr);
        }
        obj->Initialize();  // 🔥 Ahora sí se llama correctamente a Initialize()
        return obj;
    }
};

int main() {
    std::vector<int> datos = {2, 3, 4};

    auto objeto1 = AFactory::Create(datos, Tipo::Normal);   // Solo ejecuta Funcion1
    cout << "====================================" << endl;
    auto objeto2 = AFactory::Create(datos, Tipo::Especial); // Ejecuta Funcion1 y Funcion2

    return 0;
}

