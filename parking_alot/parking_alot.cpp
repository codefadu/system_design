#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

// ---------------- ENUMS ----------------

enum class VehicleType {
    BIKE,
    CAR,
    TRUCK
};

enum class SpotType {
    BIKE,
    CAR,
    TRUCK
};


// ---------------- VEHICLE ----------------

class Vehicle {
private:
    string number;
    VehicleType type;

public:
    Vehicle(string number, VehicleType type)
        : number(number), type(type) {}

    string getNumber() {
        return number;
    }

    VehicleType getType() {
        return type;
    }
};


// ---------------- PARKING SPOT ----------------

class ParkingSpot {
private:
    int id;
    SpotType type;
    Vehicle* vehicle;

public:
    ParkingSpot(int id, SpotType type)
        : id(id), type(type), vehicle(nullptr) {}

    bool isAvailable() {
        return vehicle == nullptr;
    }

    bool canFit(Vehicle* v) {
        if (v->getType() == VehicleType::BIKE)
            return type == SpotType::BIKE;

        if (v->getType() == VehicleType::CAR)
            return type == SpotType::CAR;

        if (v->getType() == VehicleType::TRUCK)
            return type == SpotType::TRUCK;

        return false;
    }

    void park(Vehicle* v) {
        vehicle = v;
    }

    void release() {
        vehicle = nullptr;
    }

    int getId() {
        return id;
    }
};


// ---------------- PARKING FLOOR ----------------

class ParkingFloor {
private:
    int floorId;
    vector<ParkingSpot*> spots;

public:
    ParkingFloor(int id)
        : floorId(id) {}

    void addSpot(ParkingSpot* spot) {
        spots.push_back(spot);
    }

    ParkingSpot* findSpot(Vehicle* vehicle) {
        for (ParkingSpot* spot : spots) {
            if (spot->isAvailable() && spot->canFit(vehicle)) {
                return spot;
            }
        }

        return nullptr;
    }
};


// ---------------- TICKET ----------------

class ParkingTicket {
private:
    int ticketId;
    Vehicle* vehicle;
    ParkingSpot* spot;
    time_t entryTime;

public:
    ParkingTicket(
        int ticketId,
        Vehicle* vehicle,
        ParkingSpot* spot
    ) : ticketId(ticketId),
        vehicle(vehicle),
        spot(spot),
        entryTime(time(nullptr)) {}

    ParkingSpot* getSpot() {
        return spot;
    }

    Vehicle* getVehicle() {
        return vehicle;
    }

    time_t getEntryTime() {
        return entryTime;
    }
};


// ---------------- PARKING SPOT MANAGER ----------------

class ParkingSpotManager {
private:
    vector<ParkingFloor*> floors;

public:
    void addFloor(ParkingFloor* floor) {
        floors.push_back(floor);
    }

    ParkingSpot* findSpot(Vehicle* vehicle) {
        for (ParkingFloor* floor : floors) {
            ParkingSpot* spot = floor->findSpot(vehicle);

            if (spot != nullptr)
                return spot;
        }

        return nullptr;
    }
};


// ---------------- PAYMENT ----------------

class Payment {
public:
    void pay(double amount) {
        cout << "Payment successful: " << amount << endl;
    }
};


// ---------------- PARKING LOT ----------------

class ParkingLot {
private:
    ParkingSpotManager spotManager;
    int nextTicketId = 1;

public:

    void addFloor(ParkingFloor* floor) {
        spotManager.addFloor(floor);
    }

    ParkingTicket* parkVehicle(Vehicle* vehicle) {

        ParkingSpot* spot = spotManager.findSpot(vehicle);

        if (spot == nullptr) {
            cout << "No suitable parking spot available\n";
            return nullptr;
        }

        spot->park(vehicle);

        ParkingTicket* ticket =
            new ParkingTicket(
                nextTicketId++,
                vehicle,
                spot
            );

        cout << "Vehicle parked at spot "
             << spot->getId() << endl;

        return ticket;
    }

    void exitVehicle(ParkingTicket* ticket) {

        time_t exitTime = time(nullptr);

        double hours =
            difftime(exitTime, ticket->getEntryTime()) / 3600.0;

        double amount = max(20.0, hours * 20.0);

        Payment payment;
        payment.pay(amount);

        ticket->getSpot()->release();

        cout << "Vehicle exited successfully\n";
    }
};


// ---------------- MAIN ----------------

int main() {

    ParkingLot parkingLot;

    // Floor 1
    ParkingFloor* floor1 = new ParkingFloor(1);

    floor1->addSpot(
        new ParkingSpot(1, SpotType::BIKE)
    );

    floor1->addSpot(
        new ParkingSpot(2, SpotType::CAR)
    );

    floor1->addSpot(
        new ParkingSpot(3, SpotType::TRUCK)
    );

    parkingLot.addFloor(floor1);


    // Vehicle enters
    Vehicle car("DL01AB1234", VehicleType::CAR);

    ParkingTicket* ticket =
        parkingLot.parkVehicle(&car);


    // Vehicle exits
    if (ticket != nullptr) {
        parkingLot.exitVehicle(ticket);
    }

    return 0;
}