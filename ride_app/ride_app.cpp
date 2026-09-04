#include <iostream>
#include <string>
#include <cmath>
#include <vector>


using namespace std;

class Location{
    double latitude;
    double longitude;
public:

    Location(double lat, double lon){
        latitude = lat;
        longitude = lon;
    }

    double  distanceTo(Location& other_location){
        // calculate distance between two locations
        double lat_diff = latitude - other_location.latitude;
        double lon_diff = longitude - other_location.longitude;
        return sqrt(lat_diff * lat_diff + lon_diff * lon_diff);
    }
};

class Vehicle{
    string vehicle_id;
    string model;
    string license_plate;

    Vehicle(string id, string mod, string plate){
        vehicle_id = id;
        model = mod;
        license_plate = plate;
    }
};

class User{
    string user_id;
    string name;
    string phone_number;

    public:
    
    User(string id, string name, string phone){
        user_id = id;
        this->name = name;
        phone_number = phone;
    }

    virtual ~User() = default;

    string getUserId() const {
        return user_id;
    }

    string getName() const {
        return name;
    }

    // additional methods for user class can be added here
};

class Rider: public User{
public:
   Rider(string id, string name, string phone): User(id, name, phone) {}
};

class Driver: public User{
    private:
    Location location;
    bool available;

    public:
    Driver(string name, Location loc, string phone): User(name, name, phone), location(loc), available(true) {}

    bool isAvailable() const {
        return available;
    }

    void setAvailability(bool status) {
        available = status;
    }

    Location getLocation() const {
        return location;
    }

};


class Ride{
    private:
    Rider& rider;
    Driver& driver;
    Location pickup_location;
    Location dropoff_location;
    bool started;

    public: 

    Ride(Rider& r, Driver& d, Location pickup, Location dropoff): rider(r), driver(d), pickup_location(pickup), dropoff_location(dropoff), started(false) {}

    void start(){
        started = true;
        cout<<"Ride started for rider: " << rider.getName() << " with driver: " << driver.getName() << endl;
    }

    void end(){
        if(!started) return;

        double distance = pickup_location.distanceTo(dropoff_location);
        double fare = 50 + distance * 10; // base fare + distance fare
        cout<<"Ride ended for rider: " << rider.getName() << " with driver: " << driver.getName() << ". Total fare: $" << fare << endl;
        driver.setAvailability(true);
    }   
};

Driver* findNearestDriver(vector<Driver*>&drivers,Location&pickup_location){
    Driver* nearest_driver = nullptr;
    double min_distance = 1e18;

    for(Driver* driver : drivers){
        if(driver->isAvailable()){
            double distance = driver->getLocation().distanceTo(pickup_location);
            if(distance < min_distance){
                min_distance = distance;
                nearest_driver = driver;
            }
        }
    }
    return nearest_driver;
}

int main() {
    cout<< "Hello, World!" << endl;

    Driver driver1("John", Location(37.7749, -122.4194), "1234567890");
    Driver driver2("Alice",Location(37.8044, -122.2711), "0987654321");

    Rider rider1("1","Ashish","1234567890");

    vector<Driver*> drivers = {&driver1, &driver2};

    Location pickup_location(37.7749, -122.4194);
    Location dropoff_location(37.8044, -122.2711);

    cout << "Finding nearest driver for rider: " << rider1.getName() << endl;
    Driver* nearest_driver = findNearestDriver(drivers, pickup_location);
    if(nearest_driver){
        cout << "Nearest driver found: " << nearest_driver->getName() << endl;
        Ride ride(rider1, *nearest_driver, pickup_location, dropoff_location);
        ride.start();
        ride.end();
    } else {
        cout << "No available drivers found for rider: " << rider1.getName() << endl;
    }       
    return 0;
}