This project implements a University Admission Portal that simulates the process of
admitting students into university branches based on JEE rank, category, preferences, and
seat availability. The system supports branch-wise reserved seats, hostel allocation (gender￾based), and mess-plan allocation. The portal is built in modern C++ using object-oriented
principles to model students, branches, hostels, mess and the admission office. The system
demonstrates modular design, clear responsibilities, and safe memory management using
smart pointers.

1. Objectives
• Model the admission workflow for multiple universities and branches, including general
and reserved seats.
• Implement eligibility checks, rank-based allocation, and preference-based branch assign￾ment.
• Provide automated hostel allocation (gender-aware) and mess-plan assignment based on
income/category.
• Produce readable reports: admission summary, branch seat status, hostel and mess status,
and eligibility report.
• Use OOP best-practices (encapsulation, inheritance, composition, RAII) and modern
C++ features.

3. Scope and Features
• Multi-university support with independent branch seat management.
• Preference-driven allocation — students are offered the first available branch from their
preference list that matches rank and seat criteria.
• Support for reserved seats (categories) and general seats per branch.
• Hostel allocation split into Boys/Girls hostels; mess allocation based on hostel allocation
and student income/category.
• Console reports for admissions, branch status, hostel/mess status and detailed student
info.
4. Architecture Overview
The system follows a simple modular architecture:
• Model layer: Classes representing domain entities: Student, Branch, University,
Hostel, Mess.
• Controller/Coordinator: AdmissionOffice – orchestrates sorting, admission, hostel
and mess allocation.
• Persistence (lightweight): In-memory objects (vectors, maps). Can be extended to
file/DB.
• Presentation: Console-based reporting functions for summaries and detailed reports.
5. Key Classes
Below are the main classes in the project and a short description of what each class represents.
Class One-line description / Role (2 short points)
Student (1) Stores all personal, academic and admission-related data
for a student (name, age, 12th%, JEE rank, category, prefer￾ences, contact).
(2) Provides eligibility checks, admission status, hostel/mess
allocation flags and printing utilities.
Mess (1) Manages mess capacity, available plans and pricing.
(2) Allocates a mess plan to a student (requires hostel alloca￾tion first) and tracks current allocations.
Branch (1) Maintains seat counts: general + reserved seats, and JEE
opening/closing rank limits.
(2) Implements branch-specific allocation rules (rank window,
reserved-category preference, decrement seat counts).
Hostel (base) (1) Generic hostel container with total rooms and allocated
rooms tracking.
(2) Provides availability queries and formatted status printing.
BoysHostel (1) Inherits Hostel; allocates rooms only to male students.
(2) Updates room counts and student hostel flag on allocation.
GirlsHostel (1) Inherits Hostel; allocates rooms only to female students.
(2) Updates room counts and student hostel flag on allocation.
University (1) Owns multiple Branch objects and tracks total seats.
(2) Implements per-university admission logic: checks rank
range, iterates student preferences and assigns seats.
2
AdmissionOffice (1) Central controller: owns universities, hostels and mess;
orchestrates full admission workflow (sorting, admitting,
allocating).
(2) Produces reports (admission summary, detailed
branch/hostel/mess status, eligibility report).
Utility functions
(printHeader/printSection)
(1) Small utilities to format console output.
(2) Improve readability of generated reports.
3
6. Detailed OOP Concept Mapping (Class-wise)
1. Student Class
Constructor:
Student::Student(string name, int income, int jee rank,
vector<string> preferred branches, string contact number,
int percentage 12th, int age, string gender, string category)
Concepts: Encapsulation, Initialization, Input Validation, Move Semantics.
Why: Initializes all data members, validates inputs, normalizes gender/category, and optimizes
memory using std::move.
Static Member:
static int Student::next roll
Concepts: Static Data Member.
Why: Ensures a unique roll number is generated across every student object.
Methods:
• bool Student::is eligible(int min percentage 12th, int min age) const
Concepts: Encapsulation, Abstraction, Const-correctness.
Why: Eligibility logic stays internal, protecting object state.
• void Student::assign roll()
Concepts: Encapsulation.
Why: Reserved for future roll assignment logic.
• void Student::set admitted(const string& branch)
Concepts: Encapsulation, State Mutation.
Why: Safely updates admission state and branch.
• bool Student::is admitted() const
Concepts: Const-correctness, Encapsulation.
• const string& Student::get name() const, Student::get income() const,
Student::get preferences() const, Student::get gender() const,
Student::get category() const, Student::get roll() const,
Student::get percentage 12th() const, Student::get age() const
Concepts: Encapsulation, Information Hiding.
4
Why: Safely exposes read-only information about the student.
• bool Student::scholarship eligible(int income threshold, const string& category pref)
const
Concepts: Abstraction.
Why: Cleanly encapsulates scholarship rules.
• void Student::allocate hostel(), bool Student::has hostel() const
Concepts: Encapsulation, State Management.
• void Student::allocate mess(const string& plan),
bool Student::has mess() const, const string& Student::get mess plan() const
Concepts: Encapsulation, Abstraction.
• void Student::print details() const, void Student::print detailed info() const
Concepts: Presentation Responsibility.
Destructor:
Student:: Student() = default;
Concepts: RAII (automatic cleanup).
2. Mess Class
Constructor:
Mess::Mess(string name, int capacity)
Concepts: Encapsulation, Initialization.
Why: Initializes mess hall name, capacity, and pricing structure.
Methods:
• bool Mess::allocate mess(Student& s, const string& plan)
Concepts: Invariant Checking, Abstraction.
Why: Ensures valid capacity and plan selection before allocation.
• bool Mess::is available() const
Concepts: Const-correctness.
• void Mess::set plan price(const string& plan, int price)
5
int Mess::get plan price(const string& plan) const
Concepts: Encapsulation, Data Protection.
• void Mess::print status() const
Concepts: Presentation.
Destructor:
Mess:: Mess() = default;
3. Branch Class
Constructor:
Branch::Branch(string name, int general seats,
unordered map<string,int> reserved, int opening rank, int closing rank)
Concepts: Encapsulation, Input Validation.
Why: Validates seat counts and rank range.
Methods:
• bool Branch::allocate seat(const Student& s)
Concepts: Abstraction, Business-rule Enforcement.
Why: Applies rank-window checks and seat logic.
• const string& Branch::get name() const
Concepts: Encapsulation.
• int Branch::seats total() const,
int Branch::general remaining() const,
int Branch::reserved remaining(const string& category) const,
const unordered map<string,int>& Branch::get reserved map() const
Concepts: Information Accessors.
• void Branch::print seat status() const
Concepts: Presentation.
Destructor:
Branch:: Branch() = default;
6
4. Hostel (Base Class)
Constructor:
Hostel::Hostel(string name, int rooms)
Concepts: Encapsulation, Initialization.
Methods:
• bool Hostel::is available() const
Concepts: Encapsulation, Const-Correctness.
• void Hostel::print status() const
Concepts: Presentation.
Design Note: Should include a virtual destructor (for safe polymorphic deletion).
5. BoysHostel (Derived)
Constructor:
BoysHostel::BoysHostel(string name, int rooms) : Hostel(name, rooms)
Concepts: Inheritance, Constructor Chaining.
Method:
void BoysHostel::allocate room(Student& s)
Concepts: Polymorphism, Encapsulation.
Why: Allocates rooms only to male students.
6. GirlsHostel (Derived)
Constructor:
GirlsHostel::GirlsHostel(string name, int rooms) : Hostel(name, rooms)
Concepts: Inheritance.
Method:
void GirlsHostel::allocate room(Student& s)
Concepts: Polymorphism, Specialization.
Why: Allocates rooms only to female students.
7
7. University Class
Static Member:
int University::total admitted global
Constructor:
University::University(string name, int opening rank, int closing rank)
Concepts: Encapsulation, Validation.
Methods:
• void University::add branch(unique ptr<Branch> b)
Concepts: Composition, RAII, Move Semantics.
• bool University::can admit(const Student& s) const
Concepts: Abstraction.
• bool University::admit student(Student& s)
Concepts: Encapsulation, Aggregation, Collaboration.
Why: Handles seat allocation, updates university and student states.
• void University::print admitted() const,
void University::print branch status() const
Concepts: Presentation.
• get branches(), get name(), get admitted count()
Concepts: Encapsulation.
Destructor:
University:: University() = default;
8. AdmissionOffice Class
Constructor:
AdmissionOffice::AdmissionOffice()
Concepts: Initialization, Composition.
8
Methods:
• void AdmissionOffice::add university(unique ptr<University> u)
Concepts: Composition, Move Semantics.
• void AdmissionOffice::set hostels(unique ptr<BoysHostel> b,
unique ptr<GirlsHostel> g)
void AdmissionOffice::set mess(unique ptr<Mess> m)
Concepts: Ownership, Composition.
• void AdmissionOffice::process applications(vector<Student>& applicants)
Concepts: Controller Pattern, Abstraction, Collaboration.
Why: Coordinates sorting, admissions, hostel & mess allocation.
• void AdmissionOffice::show all admissions() const,
void AdmissionOffice::show detailed report() const,
void AdmissionOffice::show eligibility report(const vector<Student>& applicants)
const
Concepts: Presentation.
• const vector<unique ptr<University>&> AdmissionOffice::get universities() const
Concepts: Encapsulation.
Destructor:
AdmissionOffice:: AdmissionOffice() = default;
6. Implementation Highlights
• Preference-based Admission: Applicants are sorted by JEE rank (ascending). Each
applicant is iterated in order and offered the first branch in their preference list that has
an available seat matching rank and category rules.
• Reserved seats handling: For each branch, reserved seats for categories (e.g., SC/ST/OBC)
are decremented preferentially before general seats are used.
• Hostel & Mess allocation: After admission, if hostels have rooms, a gender-appropriate
hostel is allocated. Mess allocation is then assigned based on hostel allocation and student
income/category (Basic / Standard / Premium).
• Memory safety: unique ptr for owned domain objects and raw non-owning Student*
references for admitted lists to avoid double ownership while keeping simple lifetime se￾mantics.
• Reporting: Console formatted reports for quick verification (admission summary, branch
status, hostel/mess status, eligibility report).
9
7. Build and Run
Prerequisites: This project uses a TUI (Terminal User Interface) built using the ncurses
library. NOTE: ncurses works natively on Linux systems — this is a speciality and unique
feature of our project.
Linux Build Instructions (TUI Version)
Our project contains a full NCURSES-based interactive UI. Below is the exact command sequence used to compile and run it:
# Remove old build (if any)
rm -rf build
# Compile main_ncurses.cpp with ncurses support
g++ -o admission_ncurses main_ncurses.cpp -lncurses -std=c++17
# Run the program
./admission_ncurses
Alternative Compilation (Non-TUI Version)
For systems without ncurses installed, the program can still be compiled using standard C++
source files:
g++ -std=c++17 -O2 -Wall -Wextra \
-o admission_main main.cpp university_admission.cpp
./admission_main
8. Future Work
• Add persistent storage (save/load applications, admitted lists and seat counts) using JSON
or a lightweight database (SQLite).
• Replace console UI with a web front-end or GUI to accept student applications and display
live reports.
• Add unit tests for admission rules, seat allocation and edge cases (no seats, tie ranks,
invalid preferences).
• Introduce concurrency-safe counters and locking if admission processing is moved to a
multi-threaded environment.
• Add finer-grained scheduling/policy options: multiple rounds, waitlists, seat conversion
rules.
9. Conclusion
This University Admission Portal demonstrates a compact, well-structured object-oriented design. The code cleanly separates responsibilities across classes, correctly models real-world admission entities and policies, and uses modern C++ practices such as smart pointers and move
semantics to ensure safety and performance. The design is extensible and can be expanded to
add persistence, a graphical interface, or more complex seat-allocation policies
