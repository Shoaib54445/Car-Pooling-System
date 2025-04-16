# 🚗 Car Pooling System

A console-based carpooling application built in C++ that enables users to publish, search, and book rides between cities using a location-based graph and secure user management.

## 🔐 Authentication
- New users can **sign up** with usernames and **hashed passwords**.
- Existing users can **log in** and access the carpool dashboard.
- Users can also **delete their accounts**, including associated data.

## 🛣 Ride Publishing
- Users can publish rides with:
  - Pickup and destination locations
  - Date and time (with format and validity checks)
  - Intermediate stops
  - Fare per kilometer
  - Number of available seats
- Unique Ride IDs are generated and stored with persistent data.

## 🔍 Ride Search and Booking
- Search for rides based on **pickup**, **destination**, and **date**.
- View available rides, sorted by **departure time**.
- Book seats in a selected ride and get the **total fare** calculated.
- Booked rides are recorded and available for future reference.

## 🌍 Location Graph & Recommendations
- A graph is created from predefined locations using **latitude and longitude**.
- **Dijkstra’s algorithm** with priority queues is used to calculate shortest distances.
- Trie-based location search with **autocomplete recommendations** is implemented.

## 💾 Data Persistence
- All data (users, rides, bookings) is stored in **text files**.
- Temporary file replacements ensure **safe updates** and deletions.
- Each user has a dedicated file to track their published and booked rides.

## ✅ Input Validations
- Built-in checks for valid:
  - Dates (`DD-MM-YYYY`)
  - Times (`HH:MM`)
  - Location names (with suggestions)

## 📁 Files Used
- `credentials.txt` – Stores hashed user credentials
- `rides.txt` – Stores all published ride details
- `locations.txt` – Contains predefined locations with coordinates
- `username.txt` – Per-user data including published and booked rides

## 💡 Tech Stack
- Language: C++
- Concepts: File I/O, Graphs, Dijkstra's Algorithm, Tries, Hashing, Priority Queues, Input Validation

---

> 🚧 **Note**: This is a terminal-based application. Make sure required data files (`locations.txt`, etc.) exist in the root directory before running the program.

---

## 📸 Preview
> _(You can add screenshots or GIFs of the terminal UI here)_

---

## 📜 License
MIT License (or your preferred license)
