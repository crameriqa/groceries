//
//  main.cpp
//  Groceries
//
//  Created by Braeden's UVU account on 6/11/24.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include "split.h"

using namespace std;

//
//  CUSTOMERS
//
class Customer {
public:
    int id;
    string name;
    string address;
    string city;
    string state;
    string zip;
    string phone;
    string email;
  Customer() : id(0), name(""), address(""), city(""), state(""), zip(""), phone(""), email("") {}

    // Copy constructor
    Customer(const Customer& other)
        : id(other.id), name(other.name), address(other.address), city(other.city),
          state(other.state), zip(other.zip), phone(other.phone), email(other.email) {}

    
    string print_detail() const {
        return "Customer ID #" + to_string(id) + " " + name + " " + phone + ", email: " + email + "\n" +
               address + "\n " +
               city + ", " + state + " " + zip;
    }
};

vector<Customer> customerList; // global vector of customers

int find_cust_idx(int cust_id) {
    for (int i = 0; i < customerList.size(); i++) {
        if (customerList[i].id == cust_id) {
            cout << "Customer id " << cust_id << " found." << endl;
            return i;
        }
    }
    cout << "Customer id " << cust_id << " not found." << endl;
    return -1;
}

void read_customers(string file) { // Read “customers.txt”. Put the customer records into a (global) vector.
    ifstream inFS;

    inFS.open(file);
    if (!inFS.is_open()) {
        throw runtime_error("could not open file");
    }
    string line; // entire line from file
    while (getline(inFS, line)) {
        vector<string> data = split(line, ',');
        Customer newCustomer;
        newCustomer.id = stoi(data[0]);
        newCustomer.name = data[1];
        newCustomer.address = data[2];
        newCustomer.city = data[3];
        newCustomer.state = data[4];
        newCustomer.zip = data[5];
        newCustomer.phone = data[6];
        newCustomer.email = data[7];
        cout << newCustomer.print_detail() << endl;
        customerList.push_back(newCustomer); // adds a new customer to the vector
    }
    inFS.close();
}

//
//  ITEMS & LINEITEMS
//

class Item {
public:
    int id;
    string description;
    double price;
};

class LineItem {
public:
    int item_id;
    int quantity;

    double sub_total(const vector<Item>& items) const {
        int idx = -1;
        for (int i = 0; i < items.size(); i++) {
            if (items[i].id == item_id) {
                idx = i;
                break;
            }
        }
        return (idx != -1) ? items[idx].price * quantity : 0.0;
    }

    friend bool operator<(const LineItem& item1, const LineItem& item2) {
        return item1.item_id < item2.item_id;
    }
};

vector<Item> itemList; // global vector of items

int find_item_idx(int item_id) {
    for (int i = 0; i < itemList.size(); i++) {
        if (itemList[i].id == item_id) {
            return i;
        }
    }
    return -1;
}

void read_items(string file) { // Read “items.txt”. Put the item records into a vector.
    ifstream inFS;
    inFS.open(file);
    if (!inFS.is_open()) {
        throw runtime_error("could not open file");
    }
    string line; // entire line
    while (getline(inFS, line)) {
        vector<string> data = split(line, ','); // splits the line into a vector of strings, useful for getting the data
        Item newItem;
        newItem.id = stoi(data[0]);
        newItem.description = data[1];
        newItem.price = stod(data[2]); // stod converts a string to a double ((S)tring (TO) (D)ouble)
        itemList.push_back(newItem);
    }
    inFS.close();
}

//
//  PAYMENTS
//
class Payment {
public:
    double amount;
    virtual string print_detail() const = 0; // Add a pure virtual function definition for print_detail
    virtual ~Payment() {}
    friend class Order;
};

class Credit : public Payment {
public:
    string card_number;
    string expiration;
    Credit(string number, string exp) : card_number(number), expiration(exp) {}
    ~Credit() override {}
    string print_detail() const override {
        return "Credit Card: " + card_number + " Expiration: " + expiration;
    }
};

class PayPal : public Payment {
public:
    string paypal_id;
    PayPal(string id) : paypal_id(id) {}
    ~PayPal() override {}

    string print_detail() const override {
        return "PayPal ID: " + paypal_id;
    }
};

class WireTransfer : public Payment {
public:
    string bank_id;
    string account_id;
    WireTransfer(string bank, string account) : bank_id(bank), account_id(account) {}
    ~WireTransfer() override {}
    string print_detail() const override {
        return "Bank: " + bank_id + " Account: " + account_id;
    }
};

//
// ORDERS
//

class Order {
public:
    int order_id;
    int order_number;
    string order_date;
    int customer_id;
    Payment* payment;
    Customer customer;
    vector<LineItem> line_items;

    ~Order() {
        delete payment;
    }
     // Default constructor
    Order() : order_id(0), order_number(0), order_date(""), customer_id(0), payment(nullptr) {}
 // Copy constructor
    Order(const Order& other)
        : order_id(other.order_id), order_number(other.order_number), order_date(other.order_date),
          customer_id(other.customer_id), customer(other.customer), line_items(other.line_items) {
        if (other.payment) {
            if (auto* credit = dynamic_cast<Credit*>(other.payment)) {
                payment = new Credit(*credit);
            } else if (auto* paypal = dynamic_cast<PayPal*>(other.payment)) {
                payment = new PayPal(*paypal);
            } else if (auto* wire_transfer = dynamic_cast<WireTransfer*>(other.payment)) {
                payment = new WireTransfer(*wire_transfer);
            } else {
                payment = nullptr;
            }
        } else {
            payment = nullptr;
        }
    }

    string print_order() const {
        string order_str = "Order ID: " + to_string(order_id) + " Order Date: " + order_date + "\n" +
                           customer.print_detail() + "\n";
        for (const auto& line_item : line_items) {
            order_str += "Item ID: " + to_string(line_item.item_id) + " Quantity: " + to_string(line_item.quantity) + "\n";
        }
        if (payment != nullptr) {
            order_str += "Payment: " + payment->print_detail() + "\n";
        } else {
            order_str += "Payment: None\n";
        }
        return order_str;
    }
};

list<Order> orders;

void read_orders(string file) {
    ifstream inFS;
    inFS.open(file);
    if (!inFS.is_open()) {
        throw runtime_error("could not open file");
    }
    string line1, line2;
    while (getline(inFS, line1) && getline(inFS, line2)) {
        vector<string> data1 = split(line1, ',');
        Order order = Order();
        order.order_id = stoi(data1[0]);
        order.order_number = stoi(data1[1]);
        order.order_date = data1[2];
        order.customer_id = stoi(data1[3]);

        // Find customer
        int cust_idx = find_cust_idx(order.customer_id);
        if (cust_idx != -1) {
            order.customer = customerList[cust_idx];
        }

        for (int i = 4; i < data1.size(); i++) {
            vector<string> item_data = split(data1[i], '-');
            LineItem line_item;
            line_item.item_id = stoi(item_data[0]);
            line_item.quantity = stoi(item_data[1]);
            order.line_items.push_back(line_item);
        }

        vector<string> data2 = split(line2, ',');
        int payment_type = stoi(data2[0]);

        if (payment_type == 1) {
            order.payment = new Credit(data2[1], data2[2]);
        } else if (payment_type == 2) {
            order.payment = new PayPal(data2[1]);
        } else if (payment_type == 3) {
            order.payment = new WireTransfer(data2[1], data2[2]);
        } else {
            order.payment = nullptr; // Ensure payment is initialized
        }

        // Check if the payment pointer is correctly assigned
        if (order.payment == nullptr) {
            cout << "Error: Payment not assigned for order ID " << order.order_id << endl;
        } else {
            cout << "Payment assigned for order ID " << order.order_id << endl;
        }

        orders.push_back(order);
    }
    inFS.close();
}

int main() {
    try {
        read_customers("customers.txt");
        cout << "There are " << customerList.size() << " customers" << endl;
        read_items("items.txt");
        cout << "There are " << itemList.size() << " items" << endl;
        read_orders("orders.txt");
        cout << "There are " << orders.size() << " orders" << endl;

        ofstream ofs("order_report.txt");
        for (const auto& order : orders) {
            ofs << order.print_order() << endl;
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}