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

enum class ProductType
{
    DRINK,
    SNACK,
    CHOCOLATE
};

class Product
{
private:
    int id;
    string name;
    double price;
    ProductType product_type;

public:
    Product(int id, string name, double price, ProductType type)
    {
        this->id = id;
        this->name = name;
        this->price = price;
        this->product_type = type;
    }

    double getPrice()
    {
        return price;
    }

    string getName()
    {
        return name;
    }
};

class Slot
{
private:
    int slot_id;
    Product *product;
    int quantity;

public:
    Slot(int slot_id)
    {
        this->slot_id = slot_id;
        this->product = nullptr;
        this->quantity = 0;
    }

    void addProduct(Product *product, int quantity)
    {
        this->product = product;
        this->quantity = quantity;
    }

    bool is_available()
    {
        return quantity > 0;
    }

    Product *getProduct()
    {
        return product;
    }

    int getSlotId()
    {
        return slot_id;
    }

    void dispenseProduct()
    {
        if (quantity > 0)
        {
            quantity--;
        }
    }
};

class Inventory
{
private:
    vector<Slot *> slots;

public:
    void addSlot(Slot *slot)
    {
        slots.push_back(slot);
    }

    Slot *getSlot(int slot_id)
    {
        for (Slot *slot : slots)
        {
            if (slot->getSlotId() == slot_id)
            {
                return slot;
            }
        }
        return nullptr;
    }

    void restock(int slotId, Product *product, int quantity)
    {
        for (Slot *slot : slots)
        {
            if (slot->getSlotId() == slotId)
            {
                slot->addProduct(product, quantity);
                return;
            }
        }
    }
};

enum class Denomination
{
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

class CashInventory
{
protected:
    map<Denomination, int> cash;

public:
    void addCash(Denomination denomination, int quantity)
    {
        cash[denomination] += quantity;
    }

    bool removeCash(Denomination denomination, int quantity)
    {
        if (cash[denomination] < quantity)
        {
            return false;
        }

        cash[denomination] -= quantity;
        return true;
    }

    int getTotalCash()
    {
        int total = 0;
        for (auto &entry : cash)
        {

            Denomination denomination = entry.first;
            int quantity = entry.second;

            int value = 0;

            switch (denomination)
            {

            case Denomination::COIN_1:
                value = 1;
                break;

            case Denomination::COIN_2:
                value = 2;
                break;

            case Denomination::COIN_5:
                value = 5;
                break;

            case Denomination::COIN_10:
                value = 10;
                break;

            case Denomination::NOTE_20:
                value = 20;
                break;

            case Denomination::NOTE_50:
                value = 50;
                break;

            case Denomination::NOTE_100:
                value = 100;
                break;

            case Denomination::NOTE_200:
                value = 200;
                break;

            case Denomination::NOTE_500:
                value = 500;
                break;
            }

            total += value * quantity;
        }

        return total;
    }
};

enum class PaymentType
{
    CASH,
    CARD,
    UPI
};

class Payment
{
protected:
    double amount;

public:
    Payment(double amount)
    {
        this->amount = amount;
    }

    virtual bool pay() = 0;

    virtual ~Payment() = default;

    double getAmount()
    {
        return amount;
    }
};

class CashPayment : public Payment
{
public:
    CashPayment(double amount)
        : Payment(amount)
    {
    }

    bool pay() override
    {
        cout << "Cash payment of ₹"
             << amount
             << " successful." << endl;

        return true;
    }
};

class CardPayment : public Payment
{
public:
    CardPayment(double amount)
        : Payment(amount)
    {
    }

    bool pay() override
    {
        cout << "Card payment of ₹"
             << amount
             << " successful." << endl;

        return true;
    }
};

class UPIPayment : public Payment
{
public:
    UPIPayment(double amount)
        : Payment(amount)
    {
    }

    bool pay() override
    {
        cout << "UPI payment of ₹"
             << amount
             << " successful." << endl;

        return true;
    }
};

class ChangeService
{
private:
    CashInventory *cashInventory;

public:
    ChangeService(CashInventory *cashInventory)
    {
        this->cashInventory = cashInventory;
    }

    bool canGiveChange(int amount)
    {

        if (cashInventory->getTotalCash() < amount)
        {
            return false;
        }

        return true;
    }

    void giveChange(int amount)
    {

        if (!canGiveChange(amount))
        {
            cout << "Cannot give change of ₹"
                 << amount << endl;
            return;
        }

        cout << "Returning change: ₹"
             << amount << endl;

        // Actual denomination selection
        // will be implemented here.
    }
};

class PaymentService
{
public:
    bool ProcessPayment(Payment *payment)
    {
        if (payment == nullptr)
        {
            cout << "Invalid payment." << endl;
            return false;
        }

        return payment->pay();
    }
};

enum class MachineState
{
    IDLE,
    PRODUCT_SELECTED,
    PAYMENT_PENDING,
    DISPENSING
};

class VendingMachine
{
private:
    Inventory *inventory;
    CashInventory *cashInventory;
    PaymentService *paymentService;
    ChangeService *changeService;

    MachineState state;
    Slot *selectedSlot;

    mutex transactionLock;

public:
    VendingMachine()
    {
        inventory = new Inventory();
        cashInventory = new CashInventory();
        paymentService = new PaymentService();
        changeService = new ChangeService(cashInventory);

        state = MachineState::IDLE;
        selectedSlot = nullptr;
    }

    Inventory &getInventory()
    {
        return *inventory;
    }

    void selectProduct(int slotId)
    {

        lock_guard<mutex> lock(transactionLock);

        Slot *slot = inventory->getSlot(slotId);

        if (slot == nullptr)
        {
            cout << "Invalid slot." << endl;
            return;
        }

        if (!slot->is_available())
        {
            cout << "Product is out of stock." << endl;
            return;
        }

        selectedSlot = slot;
        state = MachineState::PRODUCT_SELECTED;

        cout << "Product selected: "
             << slot->getProduct()->getName()
             << endl;
    }

    void MakePayment(Payment *payment)
    {

        lock_guard<mutex> lock(transactionLock);

        if (state != MachineState::PRODUCT_SELECTED)
        {
            cout << "Please select a product first." << endl;
            return;
        }

        if (payment == nullptr)
        {
            cout << "Invalid payment." << endl;
            return;
        }

        Product *product = selectedSlot->getProduct();

        double productPrice = product->getPrice();
        double paidAmount = payment->getAmount();

        if (paidAmount < productPrice)
        {
            cout << "Insufficient payment." << endl;
            cout << "Required: ₹" << productPrice << endl;
            cout << "Paid: ₹" << paidAmount << endl;

            state = MachineState::PRODUCT_SELECTED;
            return;
        }

        state = MachineState::PAYMENT_PENDING;

        bool success = paymentService->ProcessPayment(payment);

        if (!success)
        {
            cout << "Payment failed." << endl;
            state = MachineState::PRODUCT_SELECTED;
            return;
        }

        double change = paidAmount - productPrice;

        if (change > 0)
        {
            changeService->giveChange(change);
        }

        cout << "Payment successful." << endl;

        state = MachineState::DISPENSING;
    }

    void DispenseProduct()
    {

        lock_guard<mutex> lock(transactionLock);

        if (state != MachineState::DISPENSING)
        {
            cout << "Product cannot be dispensed." << endl;
            return;
        }

        if (selectedSlot == nullptr)
        {
            cout << "No product selected." << endl;
            return;
        }

        Product *product = selectedSlot->getProduct();

        cout << "Dispensing "
             << product->getName()
             << "..." << endl;

        selectedSlot->dispenseProduct();

        cout << "Product dispensed successfully." << endl;

        selectedSlot = nullptr;
        state = MachineState::IDLE;
    }

    void cancel()
    {

        lock_guard<mutex> lock(transactionLock);

        selectedSlot = nullptr;
        state = MachineState::IDLE;

        cout << "Transaction cancelled." << endl;
    }

    void restock(
        int slotId,
        Product *product,
        int quantity)
    {

        lock_guard<mutex> lock(transactionLock);

        inventory->restock(
            slotId,
            product,
            quantity);

        cout << "Product restocked successfully." << endl;
    }

    void collectMoney()
    {

        lock_guard<mutex> lock(transactionLock);

        int total = cashInventory->getTotalCash();

        cout << "Collected ₹"
             << total
             << " from machine."
             << endl;
    }

    ~VendingMachine()
    {

        delete inventory;
        delete cashInventory;
        delete paymentService;
        delete changeService;
    }
};

int main()
{
    VendingMachine Machine;

    // product creation
    Product coke(1, "coke", 40.0, ProductType::DRINK);
    Product silk(2, "Silk", 200.0, ProductType::CHOCOLATE);

    // create the slots

    Slot slot_1(1);
    Slot slot_2(2);

    slot_1.addProduct(&coke, 10);
    slot_2.addProduct(&silk, 5);

    Machine.getInventory().addSlot(&slot_1);
    Machine.getInventory().addSlot(&slot_2);

    Machine.selectProduct(1);
    CashPayment payment(50);
    Machine.MakePayment(&payment);
    // 40 rakh legi and baki dispense kar degi
    Machine.DispenseProduct();

    return 0;
}
