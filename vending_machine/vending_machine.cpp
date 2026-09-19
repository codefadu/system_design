// vending_machine -> have many products and slots and payment service 
// inventary
// products -> there can be different type of product
// slots -> hold the products
// payment -> can be multiple type of payments upi and all


// chat gpt version 


    //                 VendingMachine
    //                       |
    //     +-----------------+----------------+
    //     |                 |                |
    // Inventory        PaymentService    CashInventory
    //     |                 |                |
    //   Slots            Payment           Coins
    //     |                 |              Notes
    //  Product       +------+------+ 
    //                |      |      |
    //               UPI   Card    Cash

    //     PaymentService
    //           |
    //     ChangeService
    //           |
    //     CashInventory


//     VendingMachine
// │
// ├── Inventory
// │    └── Slot[]
// │         └── Product
// │
// ├── CashInventory
// │
// ├── PaymentService
// │    └── Payment
// │         ├── CashPayment
// │         ├── CardPayment
// │         └── UPIPayment
// │
// └── ChangeService
//        └── CashInventory


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>

using namespace std;

enum class ProductType {
    DRINK,
    SNACK,
    CHOCOLATE
}; 

class Product{
    private:
    int  id;
    string name;
    double price;
    ProductType product_type;

    public: 
    Product(int id, string name, double price, ProductType type);
};

class Slot{
    private:
    int slot_id;
    Product* product;
    int quantity;

    public:

    Slot(int slot_id);

    void addProduct(Product* product,int quantity);
    bool is_available();
    Product* getProduct(int product_id) ;
    void dispenseProduct();
};

class Inventory{
    private:
    vector<Slot*> slots;

    public:

    void addSlot(Slot* slot);
    Slot* getSlot(int slot_id);
    bool isAvailable();


    void restock(int slotId, Product* product , int quantity);

};


enum class Denomination {
    COIN_1,
    COIN_2,
    COIN_5,
    COIN_10,
    NOTE_20,
    NOTE_50,
    NOTE_100,
    NOTE_200,
    NOTE_500
};

class CashInventory{
    private:
    map<Denomination, int> cash;

    public:

    void  addCash(Denomination denomination,int quantity);
    bool removeCash(Denomination denomination,int quantity);
    int getTotalCash();

};


enum class PaymentType{
   CASH, CARD, UPI
};

class Payment{
    private:
    double amount;

    public:
    Payment(int amount){
        this->amount = amount;
    }
    virtual bool pay()= 0;
    virtual ~Payment() = default;

};

class CashPayment: private Payment{
    public:
    CashPayment(double amount);

    bool pay() override;
};

class CardPayment: private Payment{
    public:
    CardPayment(double amount);

    bool pay() override;
};

class UPIPayment: private Payment{
public:
    UPIPayment(double amount);

    bool pay() override;
};


class ChangeService{
    private:
    CashInventory* CashInventory;

    public:
    ChangeService(CashInventory* cashInventory);

    bool canGiveChange(double amount);
    void giveChange(double amount);
};



class PaymentService{
    public:
    bool ProcessPayment(Payment* payment);
};

enum class MachineState {
    IDLE,
    PRODUCT_SELECTED,
    PAYMENT_PENDING,
    DISPENSING
};


class VendingMachine{
    private:

    Inventory* inventory;
    CashInventory* cashInventory;
    PaymentService* paymentService;
    ChangeService* changeService;

    MachineState State;

    mutex TransactionLock;

    public: 

    VendingMachine();

    Inventory& getInventory();  

    void selectProduct(int slotId);

    void MakePayment(Payment* payment);

    void DispenseProduct();

    void cancel();

    void restock(int slotId,Product* product,int quantity);

    void collectMoney();
};

int main(){
   VendingMachine Machine;

    //product creation
    Product coke(1,"coke",40.0,ProductType::DRINK);
    Product silk(2,"Silk",200.0,ProductType::CHOCOLATE);

    // create the slots 

    Slot slot_1(1);
    Slot slot_2(2);


    slot_1.addProduct(&coke,10);
    slot_2.addProduct(&silk,5);


    Machine.getInventory().addSlot(&slot_1);
    Machine.getInventory().addSlot(&slot_2);


    Machine.restock(1,&coke,10);

    Machine.selectProduct(1);


    CashPayment payment(50);


    Machine.MakePayment(&payment);

    // 40 rakh legi and baki dispense kar degi 

    Machine.DispenseProduct();

    return 0;
}




