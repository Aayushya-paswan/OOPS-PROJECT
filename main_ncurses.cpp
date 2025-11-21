
#include "functions.cpp"
#include <ncurses.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <utility>


using namespace std::chrono_literals;
using namespace std;

AdmissionOffice office;
vector<Student> applicants;
struct BranchInfo {
    string name;
    int general;
    unordered_map<string,int> reserved;
    int opening_rank;
    int closing_rank;
};
struct UnivInfo {
    string name;
    int opening_rank;
    int closing_rank;
    vector<BranchInfo> branches;
};
vector<UnivInfo> created_univs;

template<typename T>
auto student_contact_try(const T& s, int) -> decltype(std::string(s.get_contact())) {
    return std::string(s.get_contact());
}
template<typename T>
auto student_contact_try(const T& s, long) -> decltype(std::string(s.get_contact_number())) {
    return std::string(s.get_contact_number());
}
template<typename T>
auto student_contact_try(const T& s, short) -> decltype(std::string(s.get_phone())) {
    return std::string(s.get_phone());
}
template<typename T>
auto student_contact_try(const T& s, char) -> decltype(std::string(s.get_mobile())) {
    return std::string(s.get_mobile());
}
template<typename T>
auto student_contact_try(const T& s, signed char) -> decltype(std::string(s.contact())) {
    return std::string(s.contact());
}
template<typename T>
auto student_contact_try(const T& /*s*/, unsigned) -> std::string {
    return std::string("N/A");
}

template<typename T>
static std::string get_student_contact_safe(const T& s) {
    return student_contact_try(s, 0);
}
static void pause_ms(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static vector<string> split_preferences(const string &line) {
    vector<string> out;
    string token;
    istringstream iss(line);
    while (getline(iss, token, ',')) {
        token = trim(token);
        if (!token.empty()) out.push_back(token);
    }
    return out;
}

static void safe_add_university(unique_ptr<University> u, const UnivInfo &ui) {
    office.add_university(std::move(u));
    created_univs.push_back(ui);
}

void draw_progress(WINDOW* w, int y, int x, int width, float frac) {
    int filled = std::max(0, std::min(width, (int)std::round(frac * width)));
    mvwprintw(w, y, x, "[");
    for (int i = 0; i < width; ++i) {
        mvwaddch(w, y, x+1+i, i < filled ? ACS_CKBOARD : ' ');
    }
    mvwprintw(w, y, x+1+width, "]");
    wrefresh(w);
}

void center_text(WINDOW* w, int row, const string &s, bool bold=false) {
    int cols = getmaxx(w);
    int pos = std::max(0, (cols - (int)s.length())/2);
    if (bold) wattron(w, A_BOLD);
    mvwprintw(w, row, pos, "%s", s.c_str());
    if (bold) wattroff(w, A_BOLD);
}

string input_modal(const char* title, const char* prompt, int maxlen = 200) {
    int h = 7, w = std::max(60, (int)strlen(prompt)+30);
    int starty = (LINES - h) / 2, startx = (COLS - w) / 2;
    WINDOW* win = newwin(h, w, starty, startx);
    box(win, 0, 0);
    wattron(win, A_BOLD | A_UNDERLINE);
    mvwprintw(win, 0, 2, " %s ", title);
    wattroff(win, A_BOLD | A_UNDERLINE);
    mvwprintw(win, 2, 2, "%s", prompt);
    mvwprintw(win, 4, 2, ">");
    wrefresh(win);

    echo();
    curs_set(1);
    char buffer[1024];
    mvwgetnstr(win, 4, 4, buffer, maxlen);
    noecho();
    curs_set(0);

    string out = trim(string(buffer));
    delwin(win);
    touchwin(stdscr);
    refresh();
    return out;
}

// status bar (bottom)
void set_status(const string &msg, int color_pair = 0) {
    int y = LINES - 1;
    attr_t prev_attrs = A_NORMAL;
    if (color_pair) attron(COLOR_PAIR(color_pair));
    mvhline(y, 0, ' ', COLS);
    mvprintw(y, 2, "%s", msg.c_str());
    if (color_pair) attroff(COLOR_PAIR(color_pair));
    refresh();
}

void print_table(WINDOW* w, int starty, const vector<string> &headers, const vector<vector<string>> &rows, int max_rows_display=-1, int highlight=-1) {
    int cols = getmaxx(w);
    int y = starty;
    wattron(w, A_BOLD);
    mvwprintw(w, y++, 2, "%s", headers[0].c_str());
    wattroff(w, A_BOLD);
    int colwidth = std::max(10, (cols - 6) / (int)headers.size());
    mvwprintw(w, y++, 2, "");
    int xpos = 2;
    for (size_t c = 0; c < headers.size(); ++c) {
        mvwprintw(w, y-1, xpos, "%-*s", colwidth, headers[c].substr(0, colwidth).c_str());
        xpos += colwidth + 1;
    }
    mvwaddch(w, y++, 1, ACS_HLINE);
    int rowcount = 0;
    for (size_t r = 0; r < rows.size(); ++r) {
        if (max_rows_display > 0 && rowcount >= max_rows_display) break;
        xpos = 2;
        if ((int)r == highlight) wattron(w, A_REVERSE);
        for (size_t c = 0; c < rows[r].size(); ++c) {
            mvwprintw(w, y, xpos, "%-*s", colwidth, rows[r][c].substr(0, colwidth).c_str());
            xpos += colwidth + 1;
        }
        if ((int)r == highlight) wattroff(w, A_REVERSE);
        y++; rowcount++;
    }
    wrefresh(w);
}

string reserved_to_string(const unordered_map<string,int>& m) {
    stringstream ss;
    bool first = true;
    for (auto &cat : CATEGORIES) {
        if (cat == "General") continue;
        auto it = m.find(cat);
        int v = (it == m.end() ? 0 : it->second);
        if (v > 0) {
            if (!first) ss << ",";
            ss << cat << ":" << v;
            first = false;
        }
    }
    if (first) return "None";
    return ss.str();
}

// ----------------------------- Sample data helpers -----------------------------
void createSampleUniversities() {
    // similar to your sample creation but using our globals
    {
        auto uni1 = make_unique<University>("TechVille Institute of Technology", 1, 50000);
        unordered_map<string,int> cse_reserved = { {"OBC",5}, {"SC",3}, {"ST",2}, {"EWS",2} };
        uni1->add_branch(make_unique<Branch>("Computer Science", 15, cse_reserved, 1, 5000));
        unordered_map<string,int> ece_reserved = { {"OBC",4}, {"SC",2}, {"EWS",2} };
        uni1->add_branch(make_unique<Branch>("Electronics", 12, ece_reserved, 1, 8000));
        unordered_map<string,int> mech_reserved = { {"OBC",3}, {"SC",2}, {"ST",1} };
        uni1->add_branch(make_unique<Branch>("Mechanical", 10, mech_reserved, 1, 15000));

        UnivInfo u1; u1.name = "TechVille Institute of Technology"; u1.opening_rank=1; u1.closing_rank=50000;
        u1.branches.push_back({"Computer Science",15,cse_reserved,1,5000});
        u1.branches.push_back({"Electronics",12,ece_reserved,1,8000});
        u1.branches.push_back({"Mechanical",10,mech_reserved,1,15000});
        safe_add_university(std::move(uni1), u1);
    }

    {
        auto uni2 = make_unique<University>("Global Engineering College", 1, 75000);
        unordered_map<string,int> cse_reserved2 = { {"OBC",4}, {"SC",2}, {"EWS",2} };
        uni2->add_branch(make_unique<Branch>("Computer Science", 12, cse_reserved2, 1, 3000));
        unordered_map<string,int> it_reserved = { {"OBC",3}, {"SC",2}, {"EWS",1} };
        uni2->add_branch(make_unique<Branch>("Information Technology", 10, it_reserved, 1, 7000));
        unordered_map<string,int> civil_reserved = { {"OBC",2}, {"SC",2}, {"ST",1} };
        uni2->add_branch(make_unique<Branch>("Civil Engineering", 8, civil_reserved, 1, 20000));

        UnivInfo u2; u2.name = "Global Engineering College"; u2.opening_rank=1; u2.closing_rank=75000;
        u2.branches.push_back({"Computer Science",12,cse_reserved2,1,3000});
        u2.branches.push_back({"Information Technology",10,it_reserved,1,7000});
        u2.branches.push_back({"Civil Engineering",8,civil_reserved,1,20000});
        safe_add_university(std::move(uni2), u2);
    }
}

// auto generate applicants
void autoGenerateApplicants() {
    applicants.clear();
    applicants.emplace_back("Anita Sharma", 200000, 1200,
                          vector<string>{"Computer Science","Electronics"},
                          "9991110001", 92, 18, "Female", "General");
    applicants.emplace_back("Ravi Kumar", 80000, 4200,
                          vector<string>{"Electronics","Computer Science"},
                          "9991110002", 88, 18, "Male", "OBC");
    applicants.emplace_back("Priya Singh", 50000, 2500,
                          vector<string>{"Computer Science","Information Technology"},
                          "9991110003", 94, 18, "Female", "SC");
    applicants.emplace_back("Aman Verma", 150000, 6000,
                          vector<string>{"Mechanical","Civil Engineering"},
                          "9991110004", 82, 19, "Male", "General");
    applicants.emplace_back("Sonal Gupta", 70000, 700,
                          vector<string>{"Computer Science","Electronics"},
                          "9991110005", 96, 17, "Female", "General");
    applicants.emplace_back("Karan Mehta", 30000, 1500,
                          vector<string>{"Information Technology","Computer Science"},
                          "9991110006", 90, 18, "Male", "EWS");
}

enum MenuItem {
    MENU_SAMPLE = 0,
    MENU_CUSTOM,
    MENU_ADD_APPLICANT,
    MENU_AUTOGEN,
    MENU_LIST_APPLICANTS,
    MENU_UNIV_STATUS,
    MENU_PROCESS,
    MENU_SUMMARY,
    MENU_REPORTS,
    MENU_EXPORT,
    MENU_EXIT,
    MENU_COUNT
};

const vector<string> MENU_LABELS = {
    "Create sample universities",
    "Create custom university",
    "Add applicant",
    "Auto-generate applicants",
    "List applicants",
    "Universities & branches",
    "Process applications",
    "Admission summary",
    "Detailed reports",
    "Export CSV",
    "Exit"
};

void draw_layout(WINDOW* header, WINDOW* sidebar, WINDOW* mainwin, WINDOW* footer, int selected) {
    werase(header); box(header, 0,0);
    wattron(header, A_BOLD);
    center_text(header, 1, "UNIVERSITY ADMISSION MANAGEMENT SYSTEM", true);
    wattroff(header, A_BOLD);
    mvwprintw(header, 2, 2, "Press F1 for help | F2 to refresh | q to quit");
    wrefresh(header);

    werase(sidebar); box(sidebar, 0, 0);
    mvwprintw(sidebar, 1, 2, "MENU");
    for (int i = 0; i < (int)MENU_LABELS.size(); ++i) {
        if (i == selected) wattron(sidebar, A_REVERSE);
        mvwprintw(sidebar, 3 + i, 2, "%2d) %s", i+1, MENU_LABELS[i].c_str());
        if (i == selected) wattroff(sidebar, A_REVERSE);
    }
    wrefresh(sidebar);

    werase(mainwin); box(mainwin, 0,0);
    center_text(mainwin, 1, "Welcome!", true);
    mvwprintw(mainwin, 3, 2, "Use the menu on the left or press number keys (1-%d).", (int)MENU_LABELS.size());
    mvwprintw(mainwin, 5, 2, "Universities loaded: %zu | Applicants: %zu", created_univs.size(), applicants.size());
    wrefresh(mainwin);

    werase(footer); box(footer, 0,0);
    mvwprintw(footer, 1, 2, "Status: Ready");
    wrefresh(footer);
}

// list applicants with scrolling in main window
void show_applicants_window(WINDOW* mainwin) {
    int mh = getmaxy(mainwin), mw = getmaxx(mainwin);
    werase(mainwin); box(mainwin, 0,0);
    center_text(mainwin, 1, "APPLICANTS", true);
    if (applicants.empty()) {
        mvwprintw(mainwin, 3, 2, "No applicants present. Use 'Add applicant' or 'Auto-generate'.");
        wrefresh(mainwin);
        return;
    }
    int per_page = mh - 7;
    int page = 0;
    int pages = (applicants.size() + per_page - 1) / per_page;
    keypad(mainwin, TRUE);
    int ch;
    while (1) {
        werase(mainwin); box(mainwin, 0,0);
        center_text(mainwin, 1, "APPLICANTS", true);
        int start = page * per_page;
        int y = 3;
        mvwprintw(mainwin, y++, 2, "%-6s %-20s %-6s %-6s %-4s %-8s %-8s", "Roll", "Name", "JEE", "12th%", "Age", "Gender", "Category");
        mvwaddch(mainwin, y++, 1, ACS_HLINE);
        for (int i = 0; i < per_page && start + i < (int)applicants.size(); ++i) {
            const Student &s = applicants[start + i];
            mvwprintw(mainwin, y++, 2, "%-6d %-20s %-6d %-6d %-4d %-8s %-8s",
                      s.get_roll(), s.get_name().substr(0,20).c_str(), s.jee_rank, s.get_percentage_12th(), s.get_age(),
                      s.get_gender().c_str(), s.get_category().c_str());
        }
        mvwprintw(mainwin, mh-3, 2, "Page %d/%d - Use Left/Right to change, Enter to view details, q/Esc to return", page+1, pages);
        wrefresh(mainwin);

        ch = wgetch(mainwin);
        if (ch == KEY_RIGHT) {
            if (page+1 < pages) page++;
        } else if (ch == KEY_LEFT) {
            if (page > 0) page--;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            // ask which roll to show
            string sroll = input_modal("View Student", "Enter roll number (or blank to cancel):", 10);
            if (!sroll.empty()) {
                int rr = atoi(sroll.c_str());
                bool found = false;
                for (auto &st : applicants) {
                    if (st.get_roll() == rr) {
                        // show details in a modal
                        int h = 12, w = 60;
                        int sy = (LINES - h)/2, sx = (COLS - w)/2;
                        WINDOW* win = newwin(h, w, sy, sx);
                        box(win,0,0);
                        center_text(win, 1, "Student Details", true);
                        mvwprintw(win, 3, 2, "Name: %s", st.get_name().c_str());
                        mvwprintw(win, 4, 2, "Roll: %d | JEE: %d | 12th%%: %d", st.get_roll(), st.jee_rank, st.get_percentage_12th());
                        mvwprintw(win, 5, 2, "Age: %d | Gender: %s | Category: %s", st.get_age(), st.get_gender().c_str(), st.get_category().c_str());
                        std::string contact = get_student_contact_safe(st);
                        mvwprintw(win, 6, 2, "Contact: %s", contact.c_str());
                        mvwprintw(win, 7, 2, "Preferences: ");
                        string prefs = "";
                        for (auto &p : st.get_preferences()) { if (!prefs.empty()) prefs += ", "; prefs += p; }
                        mvwprintw(win, 8, 4, "%s", prefs.c_str());
                        mvwprintw(win, 10, 2, "Press any key to close...");
                        wrefresh(win);
                        wgetch(win);
                        delwin(win);
                        touchwin(stdscr);
                        refresh();
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    set_status("Roll number not found.", 1);
                    pause_ms(900);
                }
            }
        } else if (ch == 'q' || ch == 27) {
            break;
        }
    }
    keypad(mainwin, FALSE);
}

// universities status view
void show_universities_window(WINDOW* mainwin) {
    if (created_univs.empty()) {
        werase(mainwin); box(mainwin,0,0);
        center_text(mainwin, 1, "UNIVERSITIES & BRANCHES", true);
        mvwprintw(mainwin, 3, 2, "No universities created. Use 'Create sample universities' or 'Create custom university'.");
        wrefresh(mainwin);
        wgetch(mainwin);
        return;
    }

    int mh = getmaxy(mainwin), mw = getmaxx(mainwin);
    int page = 0;
    keypad(mainwin, TRUE);

    int ch;
    while (true) {
        werase(mainwin); box(mainwin, 0, 0);
        center_text(mainwin, 1, "UNIVERSITIES & BRANCHES", true);

        int total_lines = 0;
        for (const auto& u : created_univs) {
            total_lines += 3; // University name + rank info + separator
            total_lines += u.branches.size() * 4; // Each branch takes 4 lines
        }

        int lines_per_page = mh - 7; // Reserve lines for header and navigation
        int total_pages = (total_lines + lines_per_page - 1) / lines_per_page;
        if (total_pages == 0) total_pages = 1;

        mvwprintw(mainwin, 3, 2, "Page %d/%d - Use Up/Down or PgUp/PgDn to scroll, q to return",
                 page + 1, total_pages);
        mvwprintw(mainwin, 4, 2, "-------------------------------------------------------------------");

        int y = 5;
        int lines_used = 0;
        int start_line = page * lines_per_page;
        int end_line = start_line + lines_per_page;

        for (size_t i = 0; i < created_univs.size() && y < mh - 2; ++i) {
            const auto &u = created_univs[i];

            // University header (2 lines)
            if (lines_used >= start_line && lines_used < end_line) {
                wattron(mainwin, A_BOLD);
                mvwprintw(mainwin, y++, 2, "%zu. %s", i + 1, u.name.c_str());
                wattroff(mainwin, A_BOLD);
                mvwprintw(mainwin, y++, 4, "Rank Range: %d - %d | Total Branches: %zu",
                         u.opening_rank, u.closing_rank, u.branches.size());
            }
            lines_used += 2;

            for (size_t j = 0; j < u.branches.size(); ++j) {
                const auto &b = u.branches[j];

                if (lines_used >= start_line && lines_used < end_line) {
                    wattron(mainwin, A_BOLD);
                    mvwprintw(mainwin, y++, 6, "- %s", b.name.c_str());
                    wattroff(mainwin, A_BOLD);
                    mvwprintw(mainwin, y++, 8, "General Seats: %d", b.general);
                    mvwprintw(mainwin, y++, 8, "Rank Range: %d - %d", b.opening_rank, b.closing_rank);

                    string reserved_str = reserved_to_string(b.reserved);
                    mvwprintw(mainwin, y++, 8, "Reserved: %s", reserved_str.c_str());
                }
                lines_used += 4;

                if (y >= mh - 2) break;
            }


            if (i < created_univs.size() - 1) {
                if (lines_used >= start_line && lines_used < end_line && y < mh - 2) {
                    mvwprintw(mainwin, y++, 2, "---");
                }
                lines_used += 1;
            }

            if (y >= mh - 2) break;
        }

        // Navigation help at bottom
        mvwprintw(mainwin, mh - 2, 2,
                 "Up/Down/PgUp/PgDn: Scroll  q: Return to main menu");

        wrefresh(mainwin);

        ch = wgetch(mainwin);
        switch (ch) {
            case KEY_DOWN:
                if (page + 1 < total_pages) page++;
                break;
            case KEY_UP:
                if (page > 0) page--;
                break;
            case KEY_NPAGE: // Page Down
                if (page + 1 < total_pages) page++;
                break;
            case KEY_PPAGE: // Page Up
                if (page > 0) page--;
                break;
            case ' ':
                if (page + 1 < total_pages) page++;
                break;
            case 'q':
            case 'Q':
            case 27: // ESC
                goto exit_loop;
            default:
                break;
        }
    }
    exit_loop:
    keypad(mainwin, FALSE);
}
void do_process_applications(WINDOW* mainwin) {
    if (applicants.empty()) {
        set_status("No applicants to process.", 1);
        pause_ms(900);
        return;
    }

    werase(mainwin); box(mainwin,0,0);
    center_text(mainwin, 1, "PROCESSING APPLICATIONS", true);
    mvwprintw(mainwin, 3, 2, "Processing %zu applications...", applicants.size());
    mvwprintw(mainwin, 4, 2, "Please wait. This may take a few seconds.");
    wrefresh(mainwin);

    // small animated progress
    for (int i = 0; i <= 100; i += 10) {
        draw_progress(mainwin, 6, 2, 30, i / 100.0f);
        std::this_thread::sleep_for(80ms);
    }

    std::ostringstream capture;
    std::streambuf* old_buf = std::cout.rdbuf(capture.rdbuf());

    office.process_applications(applicants);

    std::cout.rdbuf(old_buf);

    set_status("Processing complete. Admissions allocated.", 3);

    std::string output = capture.str();
    if (!output.empty()) {
        std::istringstream iss(output);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }

        // Create a scrollable results window
        int mh = getmaxy(mainwin), mw = getmaxx(mainwin);
        werase(mainwin); box(mainwin, 0, 0);
        center_text(mainwin, 1, "PROCESSING RESULTS", true);

        int per_page = mh - 6;
        int page = 0;
        int pages = (lines.size() + per_page - 1) / per_page;
        keypad(mainwin, TRUE);

        int ch;
        while (true) {
            werase(mainwin); box(mainwin, 0, 0);
            center_text(mainwin, 1, "PROCESSING RESULTS", true);

            mvwprintw(mainwin, 3, 2, "Page %d/%d -  use arrow keys to change page, q to return",
                     page + 1, pages);
            mvwaddch(mainwin, 4, 1, ACS_HLINE);

            // Display lines for current page
            int start_line = page * per_page;
            int y = 5;
            for (int i = 0; i < per_page && start_line + i < (int)lines.size(); ++i) {
                const std::string& current_line = lines[start_line + i];

                if (current_line.find("Admitted") != std::string::npos ||
                    current_line.find("SUCCESS") != std::string::npos) {
                    wattron(mainwin, COLOR_PAIR(2)); // Green for success
                } else if (current_line.find("Not admitted") != std::string::npos ||
                          current_line.find("FAIL") != std::string::npos) {
                    wattron(mainwin, COLOR_PAIR(1)); // Red for failure
                } else if (current_line.find("Student") != std::string::npos) {
                    wattron(mainwin, A_BOLD); // Bold for student names
                }

                mvwprintw(mainwin, y++, 2, "%.*s", mw - 4, current_line.c_str());

                wattroff(mainwin, COLOR_PAIR(1) | COLOR_PAIR(2) | A_BOLD);

                if (y >= mh - 2) break;
            }

            // Navigation instructions at bottom
            mvwprintw(mainwin, mh - 2, 2,
                     "q: Return to main menu");

            wrefresh(mainwin);

            ch = wgetch(mainwin);
            switch (ch) {
                case KEY_DOWN:
                case ' ':
                case 'j':
                    if (page + 1 < pages) page++;
                    break;
                case KEY_UP:
                case 'k':
                    if (page > 0) page--;
                    break;
                case KEY_NPAGE: // Page Down
                    page = std::min(pages - 1, page + 3);
                    break;
                case KEY_PPAGE: // Page Up
                    page = std::max(0, page - 3);
                    break;
                case 'q':
                case 'Q':
                case 27: // ESC
                    goto exit_loop;
                default:
                    break;
            }
        }
        exit_loop:
        keypad(mainwin, FALSE);
    } else {
        // No output captured, show simple completion message
        mvwprintw(mainwin, 8, 2, "Processing complete. Press any key to continue...");
        wrefresh(mainwin);
        wgetch(mainwin);
    }
}
// show admissions summary
void show_admission_summary(WINDOW* mainwin) {
    werase(mainwin);
    box(mainwin, 0, 0);
    center_text(mainwin, 1, "ADMISSION SUMMARY", true);

    // Debug: Show what we know about applicants
    mvwprintw(mainwin, 3, 2, "Total applicants in system: %zu", applicants.size());
    wrefresh(mainwin);
    pause_ms(500); // Brief pause to see the debug info

    if (applicants.empty()) {
        mvwprintw(mainwin, 5, 2, "No applicants found. Please add applicants first.");
        mvwprintw(mainwin, 7, 2, "Press any key to return...");
        wrefresh(mainwin);
        wgetch(mainwin);
        return;
    }

    // Try different ways to check admission status
    int admitted_count = 0;
    vector<const Student*> admitted_students;

    for (const auto& student : applicants) {
        // Try multiple possible methods to check admission status
        bool is_admitted = false;

        // Method 1: Direct method call
        is_admitted = student.is_admitted();

        // If that doesn't work, try to infer from preferences/branch allocation
        if (!is_admitted && !student.get_preferences().empty()) {
            // Check if first preference is set (as a proxy for admission)
            string first_pref = student.get_preferences()[0];
            if (!first_pref.empty() && first_pref != "N/A") {
                is_admitted = true;
            }
        }

        if (is_admitted) {
            admitted_count++;
            admitted_students.push_back(&student);
        }
    }

    // Clear and show final summary
    werase(mainwin);
    box(mainwin, 0, 0);
    center_text(mainwin, 1, "ADMISSION SUMMARY", true);

    // Show statistics
    mvwprintw(mainwin, 3, 2, "=== ADMISSION STATISTICS ===");
    mvwprintw(mainwin, 4, 4, "Total Applicants: %zu", applicants.size());
    mvwprintw(mainwin, 5, 4, "Admitted Students: %d", admitted_count);
    mvwprintw(mainwin, 6, 4, "Not Admitted: %zu", applicants.size() - admitted_count);

    if (admitted_count > 0) {
        double admission_rate = (admitted_count * 100.0) / applicants.size();
        mvwprintw(mainwin, 7, 4, "Admission Rate: %.1f%%", admission_rate);
    }

    mvwprintw(mainwin, 9, 2, "=================================");

    // Show admitted students
    int y = 11;
    if (admitted_count == 0) {
        mvwprintw(mainwin, y, 2, "No students have been admitted yet.");
        mvwprintw(mainwin, y + 2, 2, "Run 'Process Applications' first to allocate admissions.");
    } else {
        mvwprintw(mainwin, y++, 2, "ADMITTED STUDENTS:");
        mvwprintw(mainwin, y++, 2, "------------------");

        // Simple list without pagination for now
        for (const auto* student : admitted_students) {
            if (y >= getmaxy(mainwin) - 2) break;

            string branch = "Not Assigned";
            if (!student->get_preferences().empty()) {
                branch = student->get_preferences()[0];
            }

            // Truncate long names for display
            string name = student->get_name();
            if (name.length() > 25) {
                name = name.substr(0, 22) + "...";
            }

            mvwprintw(mainwin, y++, 4, "Roll: %-6d %-25s -> %s",
                     student->get_roll(), name.c_str(), branch.c_str());
        }
    }

    mvwprintw(mainwin, getmaxy(mainwin) - 2, 2, "Press any key to return to main menu...");
    wrefresh(mainwin);
    wgetch(mainwin);
}// export CSV
void export_csv_window(WINDOW* mainwin) {
    string fn = input_modal("Export", "Filename (e.g., admissions.csv):", 100);
    if (fn.empty()) {
        set_status("Export cancelled.", 3);
        return;
    }
    ofstream out(fn);
    if (!out) {
        set_status("Failed to create file: " + fn, 1);
        return;
    }
    out << "Roll,Name,JEE_Rank,12th_Percentage,Age,Gender,Category,Admitted,Branch,Hostel,Mess_Plan\n";
    for (const auto &s : applicants) {
        out << s.get_roll() << ','
            << '"' << s.get_name() << '"' << ','
            << s.jee_rank << ','
            << s.get_percentage_12th() << ','
            << s.get_age() << ','
            << s.get_gender() << ','
            << s.get_category() << ','
            << (s.is_admitted() ? "Yes" : "No") << ','
            << (s.is_admitted() ? s.get_preferences()[0] : "NA") << ','
            << (s.has_hostel() ? "Yes" : "No") << ','
            << (s.has_mess() ? s.get_mess_plan() : "No") << '\n';
    }
    out.close();
    set_status("Exported to " + fn, 2);
}

// create custom university modal
void create_custom_university(WINDOW* mainwin) {
    string uname = input_modal("New University", "University name (blank -> 'Custom University'):", 80);
    if (uname.empty()) uname = "Custom University";

    string o_s = input_modal("Opening Rank", "Opening rank (default 1):", 10);
    int o = 1; if (!o_s.empty()) o = atoi(o_s.c_str());
    string c_s = input_modal("Closing Rank", "Closing rank (default 100000):", 10);
    int c = 100000; if (!c_s.empty()) c = atoi(c_s.c_str());

    string bcount_s = input_modal("Branches", "How many branches to add (0-10):", 3);
    int bcount = 0; if (!bcount_s.empty()) bcount = atoi(bcount_s.c_str());
    bcount = std::max(0, std::min(10, bcount));

    auto uni = make_unique<University>(uname, o, c);
    UnivInfo ui; ui.name = uname; ui.opening_rank = o; ui.closing_rank = c;

    for (int i = 0; i < bcount; ++i) {
        string bname = input_modal("Branch", ("Branch " + to_string(i+1) + " name:").c_str(), 50);
        if (bname.empty()) bname = "Branch" + to_string(i+1);
        string seats_s = input_modal("Seats", ("General seats for " + bname + " (default 10):").c_str(), 6);
        int seats = 10; if (!seats_s.empty()) seats = atoi(seats_s.c_str());
        string bo_s = input_modal("Branch opening rank", "Opening rank (default 1):", 6);
        int bo = 1; if (!bo_s.empty()) bo = atoi(bo_s.c_str());
        string bc_s = input_modal("Branch closing rank", "Closing rank (default 100000):", 6);
        int bc = 100000; if (!bc_s.empty()) bc = atoi(bc_s.c_str());

        unordered_map<string,int> reserved_map;
        // ask for non-general categories
        for (auto &cat : CATEGORIES) {
            if (cat == "General") continue;
            string prompt = cat + " seats (press Enter for 0):";
            string r_s = input_modal("Reserved seats", prompt.c_str(), 6);
            if (!r_s.empty()) {
                int r = atoi(r_s.c_str());
                if (r > 0) reserved_map[cat] = r;
            }
        }
        uni->add_branch(make_unique<Branch>(bname, seats, reserved_map, bo, bc));
        ui.branches.push_back({bname, seats, reserved_map, bo, bc});
    }

    safe_add_university(std::move(uni), ui);
    set_status("Custom university added: " + uname, 2);
}

// add applicant modal
void add_applicant_modal(WINDOW* mainwin) {
    string name = input_modal("Add Applicant", "Name (blank->auto):", 80);
    if (name.empty()) name = "Student" + to_string(applicants.size()+1);
    string income_s = input_modal("Income", "Annual family income (integer, default 500000):", 15);
    int income = 500000; if (!income_s.empty()) income = atoi(income_s.c_str());
    string rank_s = input_modal("JEE Rank", "JEE Rank (integer, default 5000):", 10);
    int rank = 5000; if (!rank_s.empty()) rank = atoi(rank_s.c_str());
    string prefs = input_modal("Preferences", "Preferred branches (comma separated):", 200);
    auto preferences = split_preferences(prefs);
    if (preferences.empty()) preferences.push_back("Computer Science");
    string contact = input_modal("Contact", "Contact number (optional):", 20);
    if (contact.empty()) contact = "0000000000";
    string perc_s = input_modal("12th %", "12th percentage (integer, default 75):", 5);
    int perc = 75; if (!perc_s.empty()) perc = atoi(perc_s.c_str());
    string age_s = input_modal("Age", "Age (default 18):", 4);
    int age = 18; if (!age_s.empty()) age = atoi(age_s.c_str());
    string gender = input_modal("Gender", "Gender (Male/Female, default Male):", 10);
    if (gender.empty()) gender = "Male";
    string category = input_modal("Category", "Category (General/OBC/SC/ST/EWS, default General):", 10);
    if (category.empty()) category = "General";

    applicants.emplace_back(name, income, rank, preferences, contact, perc, age, gender, category);
    set_status("Added applicant: " + name, 2);
}

// ----------------------------- Main -----------------------------
int main() {
    // init ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // colors
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_RED, -1);
        init_pair(2, COLOR_GREEN, -1);
        init_pair(3, COLOR_CYAN, -1);
        init_pair(4, COLOR_YELLOW, -1);
        init_pair(5, COLOR_MAGENTA, -1);
    }


    int header_h = 3, footer_h = 3;
    int sidebar_w = 36;
    int main_h = LINES - header_h - footer_h;
    int main_w = COLS - sidebar_w;

    WINDOW* header = newwin(header_h, COLS, 0, 0);
    WINDOW* sidebar = newwin(main_h, sidebar_w, header_h, 0);
    WINDOW* mainwin = newwin(main_h, main_w, header_h, sidebar_w);
    WINDOW* footer = newwin(footer_h, COLS, header_h + main_h, 0);

    // initialize system defaults (hostel/mess)
    // set up facilities similar to your initializeSystem
    {
        auto boys_hostel = make_unique<BoysHostel>("Boys Hostel A", 40);
        auto girls_hostel = make_unique<GirlsHostel>("Girls Hostel B", 40);
        office.set_hostels(std::move(boys_hostel), std::move(girls_hostel));
        auto mess = make_unique<Mess>("Main Campus Mess", 60);
        mess->set_plan_price("Basic", 2000);
        mess->set_plan_price("Standard", 3000);
        mess->set_plan_price("Premium", 4500);
        office.set_mess(std::move(mess));
    }

    int selected = 0;
    draw_layout(header, sidebar, mainwin, footer, selected);
    set_status("Ready. Use arrow keys or number keys to navigate.", 3);

    bool running = true;
    while (running) {
        // draw layout with current selection
        draw_layout(header, sidebar, mainwin, footer, selected);

        int ch = getch();
        if (ch == KEY_DOWN) {
            selected = (selected + 1) % MENU_COUNT;
        } else if (ch == KEY_UP) {
            selected = (selected - 1 + MENU_COUNT) % MENU_COUNT;
        } else if (ch >= '1' && ch <= '9') {
            int idx = ch - '1';
            if (idx < MENU_COUNT) selected = idx;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            // execute selected
            werase(mainwin); box(mainwin,0,0);
            center_text(mainwin, 1, MENU_LABELS[selected], true);
            wrefresh(mainwin);

            switch (selected) {
                case MENU_SAMPLE:
                    createSampleUniversities();
                    set_status("Sample universities created.", 2);
                    break;
                case MENU_CUSTOM:
                    create_custom_university(mainwin);
                    break;
                case MENU_ADD_APPLICANT:
                    add_applicant_modal(mainwin);
                    break;
                case MENU_AUTOGEN:
                    autoGenerateApplicants();
                    set_status("Auto-generated demo applicants.", 2);
                    break;
                case MENU_LIST_APPLICANTS:
                    show_applicants_window(mainwin);
                    break;
                case MENU_UNIV_STATUS:
                    show_universities_window(mainwin);
                    break;
                case MENU_PROCESS:
                    do_process_applications(mainwin);
                    break;
                case MENU_SUMMARY:
                    show_admission_summary(mainwin);
                    break;
                case MENU_REPORTS:
                {
                    std::ostringstream capture;
                    std::streambuf* old_buf = std::cout.rdbuf(capture.rdbuf());
                    office.show_detailed_report();
                    std::cout.rdbuf(old_buf);

                    // 2. Convert captured output into lines
                    std::istringstream iss(capture.str());
                    std::vector<std::string> lines;
                    std::string line;
                    while (std::getline(iss, line)) lines.push_back(line);

                    // 3. Pause ncurses & clear its content before redrawing
                    clear();            // remove everything drawn by ncurses
                    refresh();          // update screen
                    werase(mainwin);    // clear the main window buffer
                    box(mainwin, 0, 0);
                    center_text(mainwin, 1, "DETAILED REPORTS", true);

                    int mh = getmaxy(mainwin), mw = getmaxx(mainwin);
                    int per_page = mh - 6;
                    int page = 0, pages = (lines.size() + per_page - 1) / per_page;
                    keypad(mainwin, TRUE);

                    while (true) {
                        werase(mainwin);
                        box(mainwin, 0, 0);
                        center_text(mainwin, 1, "DETAILED REPORTS", true);
                        mvwprintw(mainwin, 3, 2, "Page %d/%d to scroll, q/Esc to return.", page+1, pages);
                        mvwhline(mainwin, 4, 1, ACS_HLINE, mw-2);

                        int start = page * per_page;
                        int y = 5;
                        for (int i = 0; i < per_page && start + i < (int)lines.size(); ++i)
                            mvwprintw(mainwin, y++, 2, "%.*s", mw-4, lines[start + i].c_str());

                        wrefresh(mainwin);
                        int ch = wgetch(mainwin);
                        if (ch == KEY_RIGHT && page + 1 < pages) page++;
                        else if (ch == KEY_LEFT && page > 0) page--;
                        else if (ch == 'q' || ch == 27) break;
                    }

                    clear();
                    refresh();
                    draw_layout(header, sidebar, mainwin, footer, selected);
                    set_status("Detailed reports viewed successfully.", 3);
                    break;
                }

                case MENU_EXPORT:
                    export_csv_window(mainwin);
                    break;
                case MENU_EXIT:
                    running = false;
                    break;
            }
        } else if (ch == 'q' || ch == 'Q') {
            running = false;
        } else if (ch == KEY_F(1)) {
            // help popup
            int h = 16, w = 65;
            int sy = (LINES - h)/2, sx = (COLS - w)/2;
            WINDOW* win = newwin(h, w, sy, sx);
            box(win, 0, 0);

            // Title
            center_text(win, 1, "HELP & KEYBOARD SHORTCUTS", true);

            // Navigation section
            wattron(win, A_BOLD);
            mvwprintw(win, 3, 3, "NAVIGATION:");
            wattroff(win, A_BOLD);
            mvwprintw(win, 4, 5, "Up/Down Arrow Keys - Navigate menu");
            mvwprintw(win, 5, 5, "Enter - Select menu item");
            mvwprintw(win, 6, 5, "1-9 - Quick select menu items 1-9");
            mvwprintw(win, 7, 5, "q - Quit application");

            // Function keys section
            wattron(win, A_BOLD);
            mvwprintw(win, 9, 3, "FUNCTION KEYS:");
            wattroff(win, A_BOLD);
            mvwprintw(win, 10, 5, "F1 - Show this help screen");
            mvwprintw(win, 11, 5, "F2 - Refresh display");

            // Application info
            wattron(win, A_BOLD);
            mvwprintw(win, 13, 3, "APPLICATION:");
            wattroff(win, A_BOLD);
            //mvwprintw(win, 14, 5, "Manage universities, applicants, and admissions");

            // Close instruction at bottom
            mvwprintw(win, h-2, 2, "Press any key to close...");

            wrefresh(win);
            wgetch(win);
            delwin(win);
            touchwin(stdscr);
            refresh();
        } else if (ch == KEY_F(2)) {
            draw_layout(header, sidebar, mainwin, footer, selected);
            set_status("Screen refreshed.", 3);
        }
    }

    werase(mainwin); box(mainwin,0,0);
    center_text(mainwin, 2, "Thank you for using the Admission System", true);
    mvwprintw(mainwin, 4, 2, "Goodbye!");
    wrefresh(mainwin);
    pause_ms(600);

    // cleanup
    delwin(header); delwin(sidebar); delwin(mainwin); delwin(footer);
    endwin();
    return 0;
}

/*
# Remove old build
rm -rf build

# Compile directly without CMake
g++ -o admission_ncurses main_ncurses.cpp -lncurses -std=c++17

# Run
./admission_ncurses
 */