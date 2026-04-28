CREATE DATABASE Trypo;
GO

USE Trypo;
GO

CREATE TABLE Users (
    Id INT IDENTITY(1,1) PRIMARY KEY,
    Name NVARCHAR(255) NOT NULL,
    Password NVARCHAR(255) NOT NULL,
    Mail NVARCHAR(255) NOT NULL UNIQUE,
    PhoneNumber NVARCHAR(50),
    BirthDate DATE,
    Country NVARCHAR(100),
    Gender NVARCHAR(50),
    Address NVARCHAR(255),
    Role BIT NOT NULL -- 0 pentru client, 1 pentru admin
);
GO

CREATE TABLE RentalUnit (
    id INT IDENTITY(1,1) PRIMARY KEY,
    capacity INT NOT NULL,
    location NVARCHAR(255) NOT NULL,
    address NVARCHAR(255) NOT NULL,
    name NVARCHAR(255) NOT NULL,
    discount FLOAT DEFAULT 0.0
);
GO

CREATE TABLE Rooms (
    id INT IDENTITY(1,1) PRIMARY KEY,
    rentalUnitID INT NOT NULL,
    capacity INT NOT NULL,
    pricePerNight FLOAT NOT NULL,
    breakfast BIT DEFAULT 0,
    extraCleaning BIT DEFAULT 0,
    parking BIT DEFAULT 0,
    pool BIT DEFAULT 0,
    sauna BIT DEFAULT 0,
    AC BIT DEFAULT 0,
    balcony BIT DEFAULT 0,
    couch BIT DEFAULT 0,
    fridge BIT DEFAULT 0,
    numBeds INT NOT NULL,
    TV BIT DEFAULT 0,
    CONSTRAINT FK_Rooms_RentalUnit FOREIGN KEY (rentalUnitID) REFERENCES RentalUnit(id) ON DELETE CASCADE
);
GO

CREATE TABLE Accomodation (
    id INT IDENTITY(1,1) PRIMARY KEY,
    status NVARCHAR(50) NOT NULL CHECK (status IN ('cancelled', 'confirmed', 'finished')),
    clientId INT NOT NULL,
    rentedRoomId INT NOT NULL,
    checkIn DATE NOT NULL,
    checkOut DATE NOT NULL,
    CONSTRAINT FK_Accomodation_Users FOREIGN KEY (clientId) REFERENCES Users(id),
    CONSTRAINT FK_Accomodation_Rooms FOREIGN KEY (rentedRoomId) REFERENCES Rooms(id) ON DELETE CASCADE
);
GO

