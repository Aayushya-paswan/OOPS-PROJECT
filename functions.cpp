#include "university_admission.h"
#include<bits/stdc++.h>
using namespace std;

bool Student::is_eligible(int percentage_12th,int age){
    if(percentage_12th >= 75 && (age <= 25 && age >= 17)){
        return true;
    }
    return false;
}
university::university(){
    cout<<"Enter total seats in university: ";
    cin>>total_seats;
}
void Student::assign_roll(){
    static int roll = 1;
    roll++;

}
void Student::print_details(){
    cout<<"Name: "<<name<<endl;
    cout<<"Roll Number: "<<roll<<endl;
    cout<<"JEE Rank: "<<jee_rank<<endl;
    cout<<"12th Percentage: "<<percentage_12th<<endl;
    cout<<"Age: "<<age<<endl;
    cout<<"Contact Number: "<<contact_number<<endl;
}
bool Student::scholarship_eligible(int income, string category){
    if(category == "General" && income < 800000){
        return true;
    }
    else if((category == "OBC" || category == "SC" || category == "ST")){
        return true;
    }
    return false;
}

bool Admission_office::process_applications(Student s , university u){
    bool ans = s.is_eligible(s.percentage_12th, s.age);
    if(!ans){
        return false;
    }
    u.total_admitted++;
    if(u.total_admitted == 1){
        u.opening = u.closing = s.jee_rank;
    }
    else{
        if(s.jee_rank < u.opening){
            u.opening = s.jee_rank;
        }
        if(s.jee_rank > u.closing){
            u.closing = s.jee_rank;
        }
    }
    s.assign_roll();

}

