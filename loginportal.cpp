#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>  
#include <thread>  // For sleep_for and this_thread
#include <chrono>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <conio.h>
#include <queue>
#include <utility>

using namespace std;
#define range 15;


class Locations{
    public:
    string place;
    double lat;
    double lon;
    Locations(string p,double la,double lo){
        place=p;
        lat=la;
        lon=lo;
    }
};

class Edge{
    public:
    int src,dest;
    double wt;
    Edge(int s,int d,double w){src=s;dest=d;wt=w;}
};

class Node {
public:
    Node* children[37];
    bool eow;

    Node() {
        for (int i = 0; i < 37; i++) {
            children[i] = nullptr;
        }
        eow = false;
    }
};

int chartoidx(char ch) {
    int idx;
    if (ch >= 'a' && ch <= 'z') {
        idx = ch - 'a';  // 'a' to 'z' -> 0 to 25
    } else if (ch >= '0' && ch <= '9') {
        idx = ch - 22;   // '0' to '9' -> 26 to 35
    } else {
        idx = 36;        // For space or special characters
    }
    return idx;
}

char idxtochar(int i) {
    if (i >= 0 && i <= 25) {
        return (char)(97 + i); // 'a' to 'z'
    } else if (i >= 26 && i <= 35) {
        return (char)(22 + i); // '0' to '9'
    } else {
        return '_'; // Space
    }
}

class Trie {
public:
    Node* root;

    Trie() {
        root = new Node();
    }

    void insert(string word) {
        Node* curr = root;
        char ch;
        int idx;
        for (int i = 0; i < word.length(); i++) {
            ch = word[i];
            idx=chartoidx(ch);
            if (curr->children[idx] == nullptr) {
                curr->children[idx] = new Node();
            }
            curr = curr->children[idx];
        }
        curr->eow = true;
    }
};


double calculateDistanceInKilometer(double lat1, double lon1,
    double lat2, double lon2) {

    double latDistance = (lat1 - lat2)*M_PI/180;
    double lngDistance = (lon1 - lon2)*M_PI/180;

    double a = sin(latDistance / 2) * sin(latDistance / 2)
        + cos((lat1)*M_PI/180) * cos((lat2)*M_PI/180)
        * sin(lngDistance / 2) * sin(lngDistance / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double dis=6371*c;
    double decim=dis -(int)dis;
    decim=(int)(decim*100);
    decim=decim/100;
    dis=(int)dis+decim;
    return dis;
}


vector<Locations> loc;
unordered_map<string,int> hm1;
unordered_map<int,string> hm2;
vector<vector<Edge>> graph;
Trie t;

void creategraph(){
    vector<Locations> vec;
    ifstream inputFile("locations.txt");
    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string s1, s2,s3;
        ss >> s1 >> s2>> s3;
        t.insert(s1);
        s2=s2.substr(0,s2.size()-1);
        //cout<<s1<<" "<<stod(s2)<<" "<<stod(s3)<<endl;
        vec.push_back(Locations(s1,stod(s2),stod(s3)));
    }
    inputFile.close();
    int V=vec.size();
    for(int i=0;i<vec.size();i++){
        hm1[vec[i].place]=i;
    }
    for(int i=0;i<vec.size();i++)
    hm2[hm1[vec[i].place]]=vec[i].place;
    vector<vector<Edge>> g(V);
    for(int i=0;i<vec.size();i++){
        for(int j=i+1;j<vec.size();j++){
            g[i].push_back(Edge(i,j,calculateDistanceInKilometer(vec[i].lat,vec[i].lon,vec[j].lat,vec[j].lon)));
            g[j].push_back(Edge(j,i,calculateDistanceInKilometer(vec[i].lat,vec[i].lon,vec[j].lat,vec[j].lon)));
        }
    }
    loc=vec;
    graph=g;
}


// Function to hash passwords
string hashPassword(const string& password) {
    size_t hash = 0;
    for (char c : password) {
        hash = hash * 3 + c;
    }
    return to_string(hash);
}

// Function to sign up a new user
void signUp() {
    string username, password, confirmPassword;
    cout << "Enter username: ";
    cin >> username;

    ifstream inputFile("credentials.txt");
    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string storedUsername, storedPassword;
        ss >> storedUsername >> storedPassword;

        if (storedUsername == username) {
            cout << "Username already exists. Please try again.\n";
            return;
        }
    }
    inputFile.close();

    cout << "Enter password: ";
    cin >> password;
    cout << "Confirm password: ";
    cin >> confirmPassword;

    if (password != confirmPassword) {
        cout << "Passwords do not match. Try again.\n";
        return;
    }

    // Hash and store the credentials in the file
    ofstream outputFile("credentials.txt", ios::app);
    if (outputFile.is_open()) {
        outputFile << username << " " << hashPassword(password) << "\n";
        outputFile.close();
        cout << "Account created successfully!\n";
        string ridefilename=username+".txt";
        ofstream rideFile(ridefilename, ios::app);
        rideFile<<"PUBLISHEDRIDES"<<endl;
        rideFile<<"BOOKEDRIDES"<<endl;
    } else {
        cout << "Error: Unable to open file for writing.\n";
    }
}

// Function to log in an existing user
pair<bool,string> logIn() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    string hashedPassword = hashPassword(password);
    ifstream inputFile("credentials.txt");
    string line;

    while (getline(inputFile, line)) {
        stringstream ss(line);
        string storedUsername, storedPassword;
        ss >> storedUsername >> storedPassword;

        if (storedUsername == username && storedPassword == hashedPassword) {
            cout << "Login successful! Welcome, " << username << ".\n";
            return {true,username};
        }
    }
    inputFile.close();

    cout << "Invalid username or password. Please try again.\n";
    return {false,""};
}


// Function to delete an account
void deleteAccount() {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    string hashedPassword = hashPassword(password);
    ifstream inputFile("credentials.txt");
    ofstream tempFile("temp.txt");
    string line;
    bool accountDeleted = false;

    // Check and remove user credentials
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string storedUsername, storedPassword;
        ss >> storedUsername >> storedPassword;

        if (storedUsername == username && storedPassword == hashedPassword) {
            accountDeleted = true;
            continue; // Skip this line, i.e., delete it
        }
        tempFile << line << "\n";
    }

    inputFile.close();
    tempFile.close();

    if (accountDeleted) {
        // Remove the user's personal ride file
        string userFile = username + ".txt";
        remove(userFile.c_str());

        // Remove rides published by the user in rides.txt
        ifstream ridesFile("rides.txt");
        ofstream tempRidesFile("temp_rides.txt");

        if (!ridesFile.is_open()) {
            cout << "Error: Unable to open rides.txt.\n";
        } else {
            while (getline(ridesFile, line)) {
                stringstream ss(line);
                string rideUsername;
                ss >> rideUsername;

                if (rideUsername != username) {
                    tempRidesFile << line << "\n";
                }
            }
            ridesFile.close();
            tempRidesFile.close();

            // Replace the original rides file
            remove("rides.txt");
            rename("temp_rides.txt", "rides.txt");
        }

        // Replace the original credentials file
        remove("credentials.txt");
        rename("temp.txt", "credentials.txt");

        cout << "Account and associated rides deleted successfully!\n";
    } else {
        remove("temp.txt");
        cout << "Invalid username or password. Account not found.\n";
    }
}

void delay() {
    this_thread::sleep_for(chrono::seconds(2));  // Pause execution for 2 seconds
}

void overwritefile(string filename,string st){
    // Open the file in input mode to read the existing content
    ifstream file_in(filename);
    stringstream buffer;

    // Check if the file exists and read its content
    if (file_in.is_open()) {
        buffer << file_in.rdbuf();  // Read the file into the buffer
        file_in.close();
    }
    file_in.close();

    // Now, open the file in output mode to overwrite the content
    ofstream file_out(filename);
    
    if(file_out.is_open()) {
        if(buffer.str().size()==0){
            file_out << st;
            return;
        }
        // Write the old content after the new data
        file_out << buffer.str()<<endl;
        file_out << st;
        file_out.close();
    } else {
        cout << "Unable to open the file.\n";
    }
    file_out.close();
}





void recommendationsutil(Node* curr, string s) {
    for (int i = 0; i < 37; i++) {
        if (curr->children[i] != nullptr) {
            if (curr->children[i]->eow) {
                cout << s + idxtochar(i) << endl;
            }
            recommendationsutil(curr->children[i], s + idxtochar(i));
        }
    }
}

void recommendations(Trie* t, string word) {
    int l = word.length();
    char ch;
    int idx;
    bool hasrecom = true;
    Node* curr = t->root;

    for (int i = 0; i < l; i++) {
        ch = word[i];
        if (ch >= 'a' && ch <= 'z') {
            idx = ch - 'a';
        } else if (ch >= '0' && ch <= '9') {
            idx = ch - 22;
        } else {
            idx = 36;
        }
        if (curr->children[idx] != nullptr) {
            curr = curr->children[idx];
        } else {
            hasrecom = false;
            break;
        }
    }

    if (hasrecom) {
        if (curr->eow) {
            cout << word << endl;
        }
        recommendationsutil(curr, word);
    }
}

bool ispresent(Trie* t, string word) {
    int l = word.length();
    char ch;
    Node* curr = t->root;
    int idx;

    for (int i = 0; i < l; i++) {
        ch = word[i];
        idx = chartoidx(ch);
        if (curr->children[idx] == nullptr) {
            return false;  // If the corresponding child node is not present
        }
        curr = curr->children[idx];
    }
    return curr->eow;  // Return true if the word exists and is marked as an end of word
}

string searchplace(){
    string str="";
    char ch;
    while(true){
        ch=_getch();
        if(ch=='\r')
        break;
        if(ch == 8) {
            if (!str.empty()) {
                str.pop_back(); // Remove last character
            }
        }
        if(ch!=8)
        str=str+ch;
        system("cls");
        cout<<str<<endl;
        cout<<"Recommendations:-\n";
        if(str!="")
        recommendations(&t,str);
    }
    system("cls");
    if(ispresent(&t,str)){
        return str;
    }
    else{
        cout<<"No such place exists!"<<endl;
        delay();
        return "";
    }
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

// Function to validate the date
bool isValidDate(const string& date) {
    if (date.size() != 10 || date[2] != '-' || date[5] != '-') {
        return false; // Check basic format
    }

    int day, month, year;
    char sep1, sep2;
    stringstream ss(date);

    ss >> day >> sep1 >> month >> sep2 >> year;

    if (ss.fail() || sep1 != '-' || sep2 != '-') {
        return false; // Check parsing success
    }

    if (year < 1 || month < 1 || month > 12 || day < 1) {
        return false; // Basic range checks
    }

    // Days in each month
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isLeapYear(year)) {
        daysInMonth[1] = 29; // February has 29 days in a leap year
    }
    return day <= daysInMonth[month - 1];
}

bool isValidTime(const std::string& time) {
    // Check if the format is HH:MM
    if (time.size() != 5 || time[2] != ':') {
        return false;
    }

    // Extract hours and minutes
    std::string hoursStr = time.substr(0, 2);
    std::string minutesStr = time.substr(3, 2);

    // Ensure hours and minutes are digits
    if (!std::isdigit(hoursStr[0]) || !std::isdigit(hoursStr[1]) ||
        !std::isdigit(minutesStr[0]) || !std::isdigit(minutesStr[1])) {
        return false;
    }

    int hours = std::stoi(hoursStr);
    int minutes = std::stoi(minutesStr);

    // Check if hours and minutes are within valid ranges
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return false;
    }

    return true;
}

class Ride{
    public:
    string username;
    string uid;
    string pickup;
    string destination;
    string date;
    string dtime;
    string atime;
    double rate;
    int seats;
    int nstops;
    vector<string> stops;
};

vector<string> sortstops(string source,vector<string> &stops){
    unordered_set<int> hs;
    for(int i=0;i<stops.size();i++){
        hs.insert(hm1[stops[i]]);
    }
    vector<string> path;
    priority_queue<pair<double,int>,vector<pair<double,int>>,greater<pair<double,int>>> pq;
    int src=hm1[source];
    for(int i=0;graph[src].size();i++){
        Edge e=graph[src][i];
        if(hs.count(e.dest)){
            pq.push({e.wt,e.dest});
            hs.erase(e.dest);
        }
        if(hs.size()==0)
        break;
    }
    while(!pq.empty()){
        path.push_back(hm2[pq.top().second]);
        pq.pop();
    }
    return path;
}

void addRide(Ride &ob){
    ofstream outputFile("rides.txt", ios::app);
    outputFile <<ob.username<<" "<<ob.uid<<" "<<ob.pickup<<" "<<ob.destination<<" "<<ob.date<<" "<<ob.dtime<<" "<<ob.atime<<" "<<ob.rate<<" "<<ob.seats<<" "<<ob.nstops<<" ";
    for(int i=0;i<ob.stops.size();i++)
    outputFile<<ob.stops[i]<<" ";
    outputFile<<endl;
    outputFile.close();
    ifstream in(ob.username+".txt");
    ofstream nf("newfile.txt");
    string st;
    while(!in.eof()){
        getline(in,st);
        if(st=="BOOKEDRIDES"){
            nf<<ob.uid<<endl;
        }
        nf<<st<<endl;
    }
    in.close();
    nf.close();
    const char* charA = (ob.username+".txt").c_str();
    remove(charA);
    rename("newfile.txt", charA);
}

void publishride(string username){
    Ride ob;
    ob.username=username;
    cout<<"Ride Details:-"<<endl;
    cout<<"Pickup Place: ";
    string pick=searchplace();
    while(true){
        system("cls");
        if(pick!=""){
            system("cls");
            break;
        }
        cout<<"Pickup Place: ";
        pick=searchplace();
    }
    ob.pickup=pick;
    cout<<"Destination: ";
    string des=searchplace();
    while(true){
        system("cls");
        if(des!=""){
            system("cls");
            break;
        }
        cout<<"Destination: ";
        des=searchplace();
    }
    ob.destination=des;
    cout<<"Enter the date in DD-MM-YYYY format:"<<endl;
    string date;
    cin>>date;
    while(true){
        if(isValidDate(date)){
            break;
        }
        cout<<"Date is not valid!"<<endl;
        cout<<"Enter again: ";
        cin>>date;
    }
    ob.date=date;
    system("cls");
    cout<<"Enter the departure time in HH:MM format:"<<endl;
    string dtime;
    cin>>dtime;
    while(true){
        if(isValidTime(dtime)){
            break;
        }
        cout<<"Time is not valid!"<<endl;
        cout<<"Enter again: ";
        cin>>dtime;
    }
    ob.dtime=dtime;
    system("cls");
    cout<<"Enter the arrival time in HH:MM format:"<<endl;
    string atime;
    cin>>atime;
    while(true){
        if(isValidTime(atime)){
            break;
        }
        cout<<"Time is not valid!"<<endl;
        cout<<"Enter again: ";
        cin>>atime;
    }
    ob.atime=atime;
    cout<<"Enter the rate of ride per km: ";
    double rate;
    cin>>rate;
    ob.rate=rate;
    system("cls");
    cout<<"Enter seats: ";
    int seats;
    cin>>seats;
    ob.seats=seats;
    cout<<"Enter the number of stops: ";
    int nstops;
    cin>>nstops;
    ob.nstops=nstops;
    vector<string> stops(nstops);
    system("cls");
    cout<<"Enter the stops:-"<<endl;
    int i=0;
    while(true){
        if(i>=nstops)
        break;
        stops[i]=searchplace();
        if(stops[i]!=""){
            i++;
            continue;
        }
        system("cls");
    }
    ob.stops=stops;
    string uid=hashPassword(username+pick+des+date+dtime+to_string(nstops)).substr(0,5);
    ob.uid=uid;
    addRide(ob);//pushes the new ride to rides.txt and userfile
    cout<<"Ride Published successfully!\n";
    cout<<"Press any key to continue...\n";
    _getch();
    system("cls");
}


bool isinrange(string source,string destination){
    if(source==destination)
    return true;
    int src=hm1[source];
    int dest=hm1[destination];
    queue<int> q;
    int V=graph.size();
    vector<bool> vis(V,false);
    int curr;
    q.push(src);
    while(!q.empty()){
        curr=q.front();
        if(vis[curr]==false){
            vis[curr]=true;
            for(int i=0;i<graph[curr].size();i++){
                Edge e=graph[curr][i];
                if(e.dest==dest){
                    return e.wt<=range;
                }
                q.push(e.dest);
            }
        }
        q.pop();
    }
    return false;
}

bool isridematched(Ride &ride,string src,string dest,string date){
    if(ride.date!=date)
    return false;
    bool f1=false;
    if(isinrange(ride.pickup,src)){
        f1=true;
    }
    for(int i=0;i<ride.stops.size();i++){
        if(f1==false && isinrange(ride.stops[i],src))
        f1=true;
        if(f1==true && isinrange(ride.stops[i],dest))
        return true;
    }
    if(f1==true && isinrange(ride.destination,dest))
    return true;
    return false;
}

int pricecalculator(Ride &ride,string pickup,string destination){
    int src=hm1[pickup];
    int des=hm1[destination];
    for(int i=0;i<graph[src].size();i++){
        Edge &e=graph[src][i];
        if(e.dest==des)
        return e.wt*ride.rate;
    }
    return 0;
}

void viewBookedRides(const string& username) {
    string filename = username + ".txt";
    ifstream userFile(filename);

    if (!userFile.is_open()) {
        cout << "Error: Unable to open user file." << endl;
        return;
    }

    string line;
    bool inBookedRidesSection = false;
    vector<pair<string, int>> bookedRides; // Vector to store UID and booked seats

    // Extract UIDs and booked seats from the user's file
    while (getline(userFile, line)) {
        if (line == "BOOKEDRIDES") {
            inBookedRidesSection = true;
            continue;
        }
        if (inBookedRidesSection) {
            if (line.empty()) break;
            stringstream ss(line);
            string uid;
            int seatsBooked;
            ss >> uid >> seatsBooked; // Extract UID and booked seats
            bookedRides.push_back({uid, seatsBooked});
        }
    }
    userFile.close();

    // Read rides from rides.txt and match UIDs
    ifstream ridesFile("rides.txt");
    if (!ridesFile.is_open()) {
        cout << "Error: Unable to open rides file." << endl;
        return;
    }

    vector<Ride> matchedRides; // Vector to store matched Ride objects

    while (getline(ridesFile, line)) {
        stringstream ss(line);
        Ride ride;
        ss >> ride.username >> ride.uid >> ride.pickup >> ride.destination >> ride.date 
           >> ride.dtime >> ride.atime >> ride.rate >> ride.seats >> ride.nstops;

        // Load stops if present
        string stop;
        for (int i = 0; i < ride.nstops; ++i) {
            ss >> stop;
            ride.stops.push_back(stop);
        }

        // Check if the ride UID matches any booked UID
        for (const auto& booked : bookedRides) {
            if (ride.uid == booked.first) {
                ride.seats = booked.second; // Update seats with booked seats
                matchedRides.push_back(ride);
            }
        }
    }
    ridesFile.close();

    // Print details of matched rides
    if (matchedRides.empty()) {
        cout << "No booked rides found.\n";
        return;
    }

    cout << "\nBooked Rides for User: " << username << endl;
    for (const auto& ride : matchedRides) {
        cout << "UID: " << ride.uid
             << ", Pickup: " << ride.pickup
             << ", Departure Time: " << ride.dtime
             << ", Destination: " << ride.destination
             << ", Seats Booked: " << ride.seats << endl;
    }
}

void displayPublishedRides(const string& username) {
    system("cls");
    ifstream ridesFile("rides.txt");
    if (!ridesFile.is_open()) {
        cout << "Error: Unable to open rides.txt.\n";
        return;
    }

    string line;
    vector<Ride> userRides;

    // Read each line from rides.txt
    while (getline(ridesFile, line)) {
        stringstream ss(line);
        Ride ride;

        // Parse the ride details
        ss >> ride.username >> ride.uid >> ride.pickup >> ride.destination
           >> ride.date >> ride.dtime >> ride.atime >> ride.rate
           >> ride.seats >> ride.nstops;

        // Parse stops if available
        for (int i = 0; i < ride.nstops; ++i) {
            string stop;
            ss >> stop;
            ride.stops.push_back(stop);
        }

        // Add to the list if the username matches
        if (ride.username == username) {
            userRides.push_back(ride);
        }
    }
    ridesFile.close();

    // Display the user's rides
    if (userRides.empty()) {
        cout << "No published rides found for user: " << username << endl;
    } else {
        cout << "\nPublished Rides for User: " << username << endl;
        for (const auto& ride : userRides) {
            cout << "UID: " << ride.uid
                 << "\n   Pickup: " << ride.pickup
                 << "\n   Destination: " << ride.destination
                 << "\n   Date: " << ride.date
                 << "\n   Departure Time: " << ride.dtime
                 << "\n   Arrival Time: " << ride.atime
                 << "\n   Rate: Rs " << ride.rate << " per km"
                 << "\n   Seats Available: " << ride.seats
                 << "\n   Stops: ";
            for (const auto& stop : ride.stops) {
                cout << stop << " ";
            }
            cout << "\n--------------------------------\n";
        }
    }
    cout<<"Press any key to continue..";
    _getch();
    system("cls");
}

void searchRide(string username) {
    string pickup, destination, date;
    // Input: Pickup place
    cout << "Enter Pickup Place: ";
    pickup = searchplace();
    while (pickup.empty()) {
        cout << "Pickup Place: ";
        pickup = searchplace();
    }
    // Input: Destination place
    cout << "Enter Destination Place: ";
    destination = searchplace();
    while (destination.empty()) {
        cout << "Destination Place: ";
        destination = searchplace();
    }
    // Input: Date
    cout << "Enter the date in DD-MM-YYYY format: ";
    cin >> date;
    while (!isValidDate(date)) {
        cout << "Date is not valid! Enter again: ";
        cin >> date;
    }
    // Reading rides from the file
    ifstream inputFile("rides.txt");
    string line;
    vector<Ride> availableRides;
    cout << "\nSearching for rides...\n";
    while (getline(inputFile, line)) {
        stringstream ss(line);
        Ride ride;

        ss >> ride.username >> ride.uid >> ride.pickup >> ride.destination 
           >> ride.date >> ride.dtime >> ride.atime >> ride.rate 
           >> ride.seats >> ride.nstops;
        string stop;
        for (int i = 0; i < ride.nstops; ++i) {
            ss >> stop;
            ride.stops.push_back(stop);
        }
        // Check if the ride matches the pickup, destination, and date
        if(isridematched(ride,pickup,destination,date))
        availableRides.push_back(ride);
    }
    inputFile.close();
    sort(availableRides.begin(), availableRides.end(), [](const Ride &a, const Ride &b) {
        return a.dtime < b.dtime; // Compare departure times lexicographically (HH:MM format works here)
    });
    // Displaying matching rides
    if (availableRides.empty()) {
        cout << "No rides found for the given details.\n";
        cout<<"Press any key to continue..";
        _getch();
        system("cls");
        return;
    }
    cout << "\nAvailable Rides:\n";
    int price;
    for (int i = 0; i < availableRides.size(); ++i) {
        Ride &r = availableRides[i];
        price=pricecalculator(r,pickup,destination);
        cout << i + 1 << ". UID: " << r.uid
             << "\n   Pickup: " << r.pickup
             << "\n   Destination: " << r.destination
             << "\n   Pickup time: " << r.dtime
             << "\n   Drop time: " << r.atime
             << "\n   Price: Rs " << price
             << "\n   Seats: " << r.seats << endl;
    }
    // Booking a ride
    cout<<"Press 0 to go back\n";
    bool rideBooked = false;
    while(true){
        cout << "\nEnter the UID of the ride you want to book: ";
        string selectedUid;
        cin >> selectedUid;
        if(selectedUid=="0"){
            system("cls");
            break;
        }
        cout << "Enter the number of seats to book: ";
        int seatsToBook;
        cin >> seatsToBook;
        for (auto &ride : availableRides) {
            if (ride.uid == selectedUid ) {
                
                    rideBooked = true;
                    if(ride.seats >= seatsToBook){
                        // Update rides.txt
                        ifstream inFile("rides.txt");
                        ofstream outFile("temp.txt");
                        string fileLine;
                        ride.seats -= seatsToBook;
                        while (getline(inFile, fileLine)) {
                            if (fileLine.find(selectedUid) == string::npos) {
                                outFile << fileLine << endl;
                            } else {
                                outFile << ride.username << " " << ride.uid << " " << ride.pickup << " " << ride.destination << " "
                                        << ride.date << " " << ride.dtime << " " << ride.atime << " " << ride.rate << " "
                                        << ride.seats << " " << ride.nstops << " ";
                                for (const auto &stop : ride.stops) {
                                    outFile << stop << " ";
                                }
                                outFile << endl;
                            }
                        }

                        inFile.close();
                        outFile.close();
                        remove("rides.txt");
                        rename("temp.txt", "rides.txt");
                        cout<<"Total Price to Pay: Rs "<<seatsToBook*price<<endl;
                        cout << "Ride with UID " << selectedUid << " booked successfully!\n";
                        string str=ride.uid+" "+to_string(seatsToBook);
                        overwritefile(username+".txt",str);
                        cout<<"Press any key to continue..."<<endl;
                        system("cls");
                        return;
                    }
                    else {
                    cout << "Not enough seats available for this ride.\n";
                    break;
                    }
            } 
        }
        if (!rideBooked) {
            cout << "Invalid UID entered. Booking failed.\n";
        }
    }
}


void dashboard(string username) {
    int choice;
    do {
        cout << "\n=== Carpool Dashboard ===\n";
        cout << "1. Search a Ride\n";
        cout << "2. Publish a Ride\n";
        cout << "3. Display all booked Rides\n";
        cout << "4. Display all published Rides\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                system("cls");
                searchRide(username);
                break;
            case 2:
                system("cls");
                publishride(username);
                break;
            case 3:
                viewBookedRides(username);
                cout<<"Press any key to continue...";
                _getch();
                system("cls");
                break;
            case 4:
                displayPublishedRides(username);
                break;
            case 5:
                cout << "Exiting... Goodbye!\n";
                cout<<"Press any key to continue...\n";
                _getch();
                system("cls");
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 5);
}


// Main menu
int main() {
    system("cls");
    creategraph();
    int choice;
    do {
        cout << "=== Carpool System ===\n";
        cout << "1. Sign Up\n";
        cout << "2. Log In\n";
        cout << "3. Delete Account\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        pair<bool,string> p;
        switch (choice) {
            case 1:
                system("cls");
                signUp();
                delay();
                system("cls");
                break;
            case 2:
            system("cls");
                p=logIn();
                delay();
                system("cls");
                if(p.first)
                dashboard(p.second);
                break;
            case 3:
                system("cls");
                deleteAccount();
                delay();
                system("cls");
                break;
            case 4:
                cout << "Exiting... Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
                delay();
                system("cls");
        }
    } while (choice != 4);
    return 0;
}