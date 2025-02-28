#include <iostream>       //  1
#include <vector>         //  2
#include <unordered_map>  //  3
#include <memory>         //  4
#include <ctime>          //  5
#include <cstdlib>        //  6
#include <list>           //  7
#include <queue>          //  8
#include<string>
using namespace std;

enum SpotType {HANDICAP, BIKE,CAR, BUS};

class FeeStrategy {
    protected: 
        double rate;
    public:
        FeeStrategy(double r):rate(r){};
        double calculateFee(time_t entryTime){
            time_t now = time(nullptr);
            double duration = difftime(now, entryTime) / 3600;
            return duration * rate;
        }
        ~FeeStrategy(){};
};

class HandicapFeeStrategy : public FeeStrategy{
    public:
    HandicapFeeStrategy ():FeeStrategy(1.0){};
};

class BikeFeeStrategy: public FeeStrategy{
    public:
    BikeFeeStrategy():FeeStrategy(2.5){};
};

class CarFeeStrategy: public FeeStrategy{
    public:
    CarFeeStrategy():FeeStrategy(5.0){};
};

class BusFeeStrategy: public FeeStrategy{
    public:
    BusFeeStrategy():FeeStrategy(10.0){};
};

class Ticket{
    public:
    string ticketNumber;
    time_t entryTime;
    Ticket( ){
        this->ticketNumber = to_string(rand());
        this->entryTime = time(nullptr);
    };
};

class Vehicle{
    public:
    string vehicleNumber;
    SpotType type;
    FeeStrategy* feeStrategy;
    Ticket* ticket;

    Vehicle(string vehicleNumber, SpotType type):vehicleNumber(vehicleNumber),type(type){
        switch (type)
        {
        case HANDICAP:
            feeStrategy = new HandicapFeeStrategy ();
            break;
        case BIKE:
            feeStrategy = new BikeFeeStrategy();
            break;
        case CAR:
            feeStrategy = new CarFeeStrategy();
            break;
        case BUS:
            feeStrategy = new BusFeeStrategy();
            break;
        default:
            break;
        }
    }
};

class Spot{
    public:
    string id;
    bool isOccupied;
    Vehicle* vehicle;
    SpotType type;
    Spot(string id, SpotType type):id(id),type(type),isOccupied(false),vehicle(nullptr){};

};

class ParkingLot{

    private:
    string levelId;
    unordered_map<SpotType,queue<Spot*>> availableSpots;
    vector<Spot*> allSpots;

    void createSpot(SpotType type, int i){
        string type_id = "";
        switch(type){
            case BIKE:
                type_id = "B";
                break;
            case HANDICAP:
                type_id = "H";
                break;
            case CAR:
                type_id="C";
                break;
            case BUS:
                type_id="BU";
            break;
        }

        Spot* spot = new Spot(levelId + type_id + to_string(i+1), type);
        allSpots.push_back(spot);
        availableSpots[type].push(spot);
    }
    
    public:
    ParkingLot(string levelId,int handicap, int bike,int car, int bus):levelId(levelId){
        for( int i=0; i<handicap; i++) createSpot(SpotType::HANDICAP,i);
        for( int i=0; i<bike; i++) createSpot(SpotType::BIKE,i);
        for( int i=0; i<car; i++) createSpot(SpotType::CAR,i);   
        for( int i=0; i<bus; i++) createSpot(SpotType::BUS,i);   
    }

    Spot* findAvailableSpot(SpotType type){
        if(availableSpots[type].empty())
            return nullptr;
        
        Spot* spot = availableSpots[type].front();
        availableSpots[type].pop();
        return spot;
    }

    bool parkVehicle(Vehicle* vehicle){
        Spot* spot = findAvailableSpot(vehicle->type);
        Ticket* ticket = new Ticket();

        if(spot){
            spot->isOccupied= true;
            spot->vehicle = vehicle;
            vehicle->ticket = ticket;
            cout<< "Vehicle parked : "<< vehicle->vehicleNumber << endl;
            return true;
        }
        return false;
    }

    bool removeVehicle(Vehicle* vehicle){
        for(auto spot: allSpots){
            if(spot->isOccupied && spot->vehicle == vehicle){
                spot->isOccupied = false;

                double cost = vehicle->feeStrategy->calculateFee(time(nullptr));
                cout << "Money to be recieved "<< cost << endl;
                delete spot->vehicle;

                availableSpots[spot->type].push(spot);
                return true;
                break;
            }
        }
        return false;
    }
};

int main(){
    ParkingLot parkinglot("1",10,10,10,10);

    Vehicle* car1 =  new Vehicle("ABC",SpotType::CAR);
    Vehicle* car2 = new Vehicle("ABCC",SpotType::CAR);
    Vehicle* bike = new Vehicle("ASASDAsd",SpotType::BIKE);

    parkinglot.parkVehicle(car1);
    parkinglot.parkVehicle(car2);
    parkinglot.parkVehicle(bike);

    parkinglot.removeVehicle(car1);
    return 1;
}