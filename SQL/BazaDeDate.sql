CREATE DATABASE Trypo

USE Trypo
CREATE TABLE Users (
    -- ID-ul se va incrementa automat la fiecare inserare (1, 2, 3...)
    Id INT PRIMARY KEY IDENTITY(1,1), 
    
    -- NVARCHAR(255) permite caractere speciale și lungime variabilă
    Nume NVARCHAR(100) NOT NULL,
    Parola NVARCHAR(255) NOT NULL,
    Email NVARCHAR(150) UNIQUE NOT NULL, -- UNIQUE asigură că nu avem dubluri
    Telefon NVARCHAR(20),
    
    -- Data nasterii sub forma de tip DATE (format standard: YYYY-MM-DD)
    DataNastere DATE,
    
    Tara NVARCHAR(50),
    Gen NVARCHAR(15),
    Adresa NVARCHAR(MAX) -- MAX permite texte foarte lungi
);

Delete from Users
where Id=1