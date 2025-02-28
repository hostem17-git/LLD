#include <iostream>       //  1
#include <vector>         //  2
#include <unordered_map>  //  3
#include <memory>         //  4
#include <ctime>          //  5
#include <cstdlib>        //  6
#include <list>           //  7
#include <queue>          //  8
#include <string>
using namespace std;

// ========================= Strategy Pattern: Fee Calculation =========================
class ParkingFeeStrategy {
public:
    virtual double calculateFee(time_t entryTime) = 0;  //  9
    virtual ~ParkingFeeStrategy() {}  //  10
};

class HourlyRate : public ParkingFeeStrategy {
private:
    double rate;
public:
    HourlyRate(double r) : rate(r) {}  //  11
    double calculateFee(time_t entryTime) override {
        time_t now = time(nullptr);
        double duration = difftime(now, entryTime) / 3600;  // Convert seconds to hours
        return duration * rate;  //  12
    }
};

// ========================= Factory Pattern: Vehicle Creation =========================
enum class VehicleType { CAR, BIKE, BUS };

class Vehicle {
public:
    string licensePlate;
    VehicleType type;
    ParkingFeeStrategy* feeStrategy;

    Vehicle(string licensePlate, VehicleType type) : licensePlate(licensePlate), type(type), feeStrategy(nullptr) {}

    virtual ~Vehicle() {
        delete feeStrategy;  // Prevent memory leak
    }  // 13
};

class Car : public Vehicle {
public:
    Car(string licensePlate) : Vehicle(licensePlate, VehicleType::CAR) {
        feeStrategy = new HourlyRate(5.0);  // 14
    }
};

class Bike : public Vehicle {
public:
    Bike(string licensePlate) : Vehicle(licensePlate, VehicleType::BIKE) {
        feeStrategy = new HourlyRate(2.0);  // 15
    }
};

class Bus : public Vehicle {
public:
    Bus(string licensePlate) : Vehicle(licensePlate, VehicleType::BUS) {
        feeStrategy = new HourlyRate(10.0);  // 16
    }
};

class VehicleFactory {
public:
    static Vehicle* createVehicle(VehicleType type, string licensePlate) {
        switch (type) {
            case VehicleType::CAR: return new Car(licensePlate);
            case VehicleType::BIKE: return new Bike(licensePlate);
            case VehicleType::BUS: return new Bus(licensePlate);
            default: return nullptr;  // 17
        }
    }
};

// ========================= Parking Spot Types =========================
enum class SpotType { HANDICAP, COMPACT, LARGE, MOTORCYCLE };

class ParkingSpot {
public:
    string spotId;
    SpotType type;
    bool isOccupied;
    Vehicle* vehicle;

    ParkingSpot(string spotId, SpotType type) : spotId(spotId), type(type), isOccupied(false), vehicle(nullptr) {}  // 18
};

// ========================= Multi-Level Parking System =========================
class ParkingLevel {
private:
    string levelId;
    unordered_map<SpotType, queue<ParkingSpot*>> availableSpots;
    vector<ParkingSpot> allSpots;

public:
    ParkingLevel(string levelId, int handicap, int compact, int large, int motorcycle) : levelId(levelId) {
        for (int i = 0; i < handicap; i++) allSpots.push_back(ParkingSpot(levelId + "-H" + to_string(i+1), SpotType::HANDICAP));
        for (int i = 0; i < compact; i++) allSpots.push_back(ParkingSpot(levelId + "-C" + to_string(i+1), SpotType::COMPACT));
        for (int i = 0; i < large; i++) allSpots.push_back(ParkingSpot(levelId + "-L" + to_string(i+1), SpotType::LARGE));
        for (int i = 0; i < motorcycle; i++) allSpots.push_back(ParkingSpot(levelId + "-M" + to_string(i+1), SpotType::MOTORCYCLE));

        for (auto &spot : allSpots) availableSpots[spot.type].push(&spot);  // 19
    }

    ParkingSpot* findAvailableSpot(VehicleType vType) {
        SpotType requiredType = (vType == VehicleType::BIKE) ? SpotType::MOTORCYCLE :
                                (vType == VehicleType::CAR) ? SpotType::COMPACT :
                                SpotType::LARGE;  

        if (!availableSpots[requiredType].empty()) {
            ParkingSpot* spot = availableSpots[requiredType].front();
            availableSpots[requiredType].pop();
            return spot;  // 20
        }
        return nullptr;  // 21
    }

    bool parkVehicle(Vehicle* vehicle) {
        ParkingSpot* spot = findAvailableSpot(vehicle->type);
        if (!spot) return false;  // 22

        spot->isOccupied = true;
        spot->vehicle = vehicle;
        cout << "[INFO] Vehicle " << vehicle->licensePlate << " parked at " << spot->spotId << endl;
        return true;
    }

    bool removeVehicle(string licensePlate) {
        for (auto &spot : allSpots) {
            if (spot.isOccupied && spot.vehicle->licensePlate == licensePlate) {
                availableSpots[spot.type].push(&spot);
                spot.isOccupied = false;
                cout << "[INFO] Vehicle " << licensePlate << " exited from " << spot.spotId << endl;
                delete spot.vehicle;  // Free memory  
                spot.vehicle = nullptr;
                return true;
            }
        }
        return false;  // 23
    }
};

// ========================= Parking Lot with Multi-Level Support =========================
class ParkingLot {
private:
    unordered_map<string, ParkingLevel*> levels;

public:
    ParkingLot(vector<string> levelIds, int handicap, int compact, int large, int motorcycle) {
        for (const auto &id : levelIds) {
            levels[id] = new ParkingLevel(id, handicap, compact, large, motorcycle);  // 24
        }
    }

    ~ParkingLot() {
        for (auto &level : levels) {
            delete level.second;
        }
    }

    bool parkVehicle(Vehicle* vehicle) {
        for (auto &level : levels) {
            if (level.second->parkVehicle(vehicle)) return true;
        }
        cout << "[WARNING] Parking Full for " << vehicle->licensePlate << endl;
        delete vehicle;  // Free memory if parking fails
        return false;  // 25
    }

    bool removeVehicle(string licensePlate) {
        for (auto &level : levels) {
            if (level.second->removeVehicle(licensePlate)) return true;
        }
        cout << "[WARNING] Vehicle not found: " << licensePlate << endl;
        return false;  // 26
    }
};

// ========================= Main Function (Test Case) =========================
int main() {
    ParkingLot parkingLot({"L1", "L2"}, 5, 20, 10, 10);  // 27  // 2 levels, spot distribution

    Vehicle* car1 = VehicleFactory::createVehicle(VehicleType::CAR, "KA-01-1234");  // 28
    Vehicle* bike1 = VehicleFactory::createVehicle(VehicleType::BIKE, "KA-02-5678");  // 29
    Vehicle* bus1 = VehicleFactory::createVehicle(VehicleType::BUS, "KA-03-9999");  // 30

    parkingLot.parkVehicle(car1);  // 31
    parkingLot.parkVehicle(bike1);  // 32
    parkingLot.parkVehicle(bus1);  // 33

    parkingLot.removeVehicle("KA-01-1234");  // 34

    return 0;  // 35
}
