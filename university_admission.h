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
