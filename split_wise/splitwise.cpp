#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ==================== User ====================

class User {
private:
    int id;
    string name;

public:
    User(int id, string name);
};


// ==================== Split ====================

class Split {
protected:
    User* user;
    double amount;

public:
    Split(User* user);
    virtual ~Split() = default;
};


// ==================== Split Types ====================

class EqualSplit : public Split {
public:
    EqualSplit(User* user);
};


class ExactSplit : public Split {
public:
    ExactSplit(User* user, double amount);
};


class PercentageSplit : public Split {
private:
    double percentage;

public:
    PercentageSplit(User* user, double percentage);
};


// ==================== Expense ====================

class Expense {
private:
    int id;
    double amount;
    User* paidBy;
    vector<Split*> splits;

public:
    Expense(
        int id,
        double amount,
        User* paidBy,
        vector<Split*> splits
    );
};


// ==================== Group ====================

class Group {
private:
    int id;
    string name;
    vector<User*> members;
    vector<Expense*> expenses;

public:
    Group(int id, string name);
};


// ==================== Expense Manager ====================

class ExpenseManager {
private:
    vector<User*> users;
    vector<Group*> groups;

public:
    ExpenseManager();
};