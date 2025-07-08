# StealthDBMS Example Commands

## Basic Table Operations

# Create tables

make table employee fields last, first, dep, salary, year
make table student fields fname, lname, major, age, company

# Insert data into employee table

insert into employee values Blow, Joe, CS, 100000, 2018
insert into employee values Blow, JoAnn, Physics, 200000, 2016
insert into employee values Johnson, Jack, HR, 150000, 2014
insert into employee values Johnson, "Jimmy", Chemistry, 140000, 2018

# Insert data into student table

insert into student values Flo, Yao, CS, 20, Google
insert into student values Bo, Yang, CS, 28, Microsoft
insert into student values "Sammuel L.", Jackson, CS, 40, Uber
insert into student values "Flo", "Jackson", Math, 21, Google
insert into student values "Greg", "Pearson", Physics, 20, Amazon

## Query Operations

# Select all records

select _ from employee
select _ from student

# Select specific fields

select fname, lname from student
select last, first, salary from employee

# Select with WHERE conditions

select _ from student where age > 25
select _ from employee where salary > 150000
select fname, lname from student where major = CS
select \* from student where lname = Jackson

# Complex WHERE clauses

select lname, fname, major from student where ((lname=Yang or major=CS) and age<23) or lname=Jackson

## Table Management

# Show all tables

show tables

# Drop tables (use with caution!)

drop table employee
drop table student

## Tips

# - Use quotes around values with spaces: "Sammuel L."

# - Commands are case-insensitive

# - Use 'exit' to quit the program
