#include<bits/stdc++.h>
using namespace std;

class university{
    private:
        int closing, opening;
    public:
        bool can_admit(Student&);
        ~university();
};

class Student{
    private:
        static int total_students;
        string name;
        int jee_rank;
        int income;
        vector<string> preferred_branch;
        string contact_number;
        int percentage_12th;
        int age;
        string gender;
        string category;
    public:
        Student(string, int, int, vector<string>, string, int, int, string, string);
        bool is_eligible(int, int);
        void assign_roll();
        void print_details();
        bool scholarship_eligible(int, string);
        ~Student();
};

class branch{
    private:
        string Branch;
        int seats_available;
        int closing, opening;
    public:
        branch(string, int, int, int);
        bool allocate_seat(Student&);
        ~branch();
};

class Hostel{
    private:
        
    public:
        Hostel(string, int, int);
        bool allocate_hostel(Student&);
        ~Hostel();
};