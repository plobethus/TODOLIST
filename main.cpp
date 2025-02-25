// Program written by Henry Moran

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>

//========================================
// Structures
//========================================
struct Assignment {
    std::string className;
    std::string assignmentName;
    std::string dueDate;  // Format: "3/3/3"
    std::string dueTime;  // Format: "11:59PM" 
};

struct Exam {
    std::string className;
    std::string examName;
    std::string examDate; // Format: "3/3/3"
};

//========================================
// Utility Functions
//========================================

/**
 * Recursively checks user input within a given range.
 */
int checkInput(int range) {
    int input;
    std::cin >> input;

    if (!std::cin || input < 1 || input > range) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid choice. Please enter a number from 1 to " 
                  << range << ": ";
        return checkInput(range);
    }

    std::cin.ignore(1000, '\n');
    return input;
}

/**
 * Maps numeric month to a readable month name.
 */
std::string getMonthName(int month) {
    std::string months[] = { "January", "February", "March", "April", "May", "June",
                             "July", "August", "September", "October", "November", "December" };
    if (month >= 1 && month <= 12) {
        return months[month - 1];
    }
    return "Unknown";
}

/**
 * Converts a numeric date format (3/3/3) to "March 3rd 2003".
 */
std::string formatDate(const std::string& date) {
    std::istringstream ss(date);
    int month, day, year;
    char slash1, slash2;

    if (!(ss >> month >> slash1 >> day >> slash2 >> year) || slash1 != '/' || slash2 != '/') {
        return "Invalid Date";
    }

    // If the year is a 2-digit number, always add 2000
    if (year < 100) {  
        year += 2000;
    }

    // Determine ordinal suffix
    std::string suffix = "th";
    if (day == 1 || day == 21 || day == 31) suffix = "st";
    else if (day == 2 || day == 22) suffix = "nd";
    else if (day == 3 || day == 23) suffix = "rd";

    return getMonthName(month) + " " + std::to_string(day) + suffix + " " + std::to_string(year);
}


/**
 * Splits a string by a specified delimiter and returns tokens in a vector.
 */
void splitString(const std::string& str, char delimiter, std::vector<std::string>& tokens) {
    tokens.clear();
    std::istringstream ss(str);
    std::string temp;
    while (std::getline(ss, temp, delimiter)) {
        if (!temp.empty()) {
            tokens.push_back(temp);
        }
    }
}

//========================================
// Core Functions
//========================================

/**
 * Reads the schedule from a file.
 */
bool readSchedule(const std::string& filename,
                  std::vector<std::string>& classes,
                  std::vector<Assignment>& assignments,
                  std::vector<Exam>& exams) 
{
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    std::string line;
    std::string section;
    bool inSchedule = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line == "BEGIN") {
            inSchedule = true;
            continue;
        }
        if (line == "END") break;

        if (!inSchedule) continue;

        if (line == "# Classes") {
            section = "Classes";
            continue;
        } 
        else if (line == "# Assignments") {
            section = "Assignments";
            continue;
        }
        else if (line == "# Exams") {
            section = "Exams";
            continue;
        }

        // Process sections
        if (section == "Classes") {
            classes.push_back(line);
        }
        else if (section == "Assignments") {
            std::vector<std::string> tokens;
            splitString(line, '|', tokens);

            if (tokens.size() < 3) {
                std::cerr << "Invalid assignment format: " << line << std::endl;
                continue;
            }

            Assignment a;
            a.className = tokens[0];
            a.assignmentName = tokens[1];
            a.dueDate = tokens[2];
            a.dueTime = (tokens.size() > 3) ? tokens[3] : "";  // If time exists

            assignments.push_back(a);
        }
        else if (section == "Exams") {
            std::vector<std::string> tokens;
            splitString(line, '|', tokens);

            if (tokens.size() < 3) {
                std::cerr << "Invalid exam format: " << line << std::endl;
                continue;
            }

            Exam e;
            e.className = tokens[0];
            e.examName = tokens[1];
            e.examDate = tokens[2];

            exams.push_back(e);
        }
    }

    file.close();
    return true;
}

/**
 * Prints schedule in human-readable format.
 */
void printSchedule(const std::vector<std::string>& classes,
                   const std::vector<Assignment>& assignments,
                   const std::vector<Exam>& exams) 
{
    std::cout << "\n--- Current Schedule ---\n\n";

    std::cout << "# Classes:\n";
    for (size_t i = 0; i < classes.size(); ++i) {
        std::cout << " - " << classes[i] << "\n";
    }

    std::cout << "\n# Assignments:\n";
    for (size_t i = 0; i < assignments.size(); ++i) {
        std::cout << " - " << assignments[i].className 
                  << " | " << assignments[i].assignmentName
                  << " | Due on " << formatDate(assignments[i].dueDate);
        if (!assignments[i].dueTime.empty()) {
            std::cout << " at " << assignments[i].dueTime;
        }
        std::cout << "\n";
    }

    std::cout << "\n# Exams:\n";
    for (size_t i = 0; i < exams.size(); ++i) {
        std::cout << " - " << exams[i].className 
                  << " | " << exams[i].examName
                  << " | Exam on " << formatDate(exams[i].examDate) << "\n";
    }
}

//========================================
// Main Function
//========================================

int main() {
    std::vector<std::string> classes;
    std::vector<Assignment> assignments;
    std::vector<Exam> exams;

    std::string scheduleFile = "schedule.txt";

    if (!readSchedule(scheduleFile, classes, assignments, exams)) {
        std::cerr << "Failed to read schedule. Exiting...\n";
        return 1;
    }

    printSchedule(classes, assignments, exams);

    return 0;
}