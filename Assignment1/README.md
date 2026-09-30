# Assignment 1 – C Fundamentals

This assignment implements two programs in C:

## 1. C Expression Calculator

A simple expression calculator that performs arithmetic operations.

### Features

- Addition
- Subtraction
- Multiplication
- Division
- Multi-digit numbers
- Operator precedence
- Division by zero handling
- Invalid expression validation

---

## 2. CRUD Operations using File Handling

A user management program that performs CRUD operations using file handling in C.

### Features

- Create a new user
- Read and display all users
- Update an existing user
- Delete a user
- Stores user data in a file (`users.txt`)
- Prevents duplicate user IDs
- Handles user-not-found cases

### User Details

Each user contains:

- ID
- Name
- Age

### File Used

`users.txt` is used to store the user records.

### CRUD Operations

1. **Create User** – Adds a new user to the file.
2. **Read Users** – Displays all stored users.
3. **Update User** – Updates the details of an existing user.
4. **Delete User** – Deletes a user from the file.

---

## Project Structure

```text
Assignment1/
│
├── README.md
├── calculator.c
├── crud.c
└── users.txt
