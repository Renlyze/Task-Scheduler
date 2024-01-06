#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <regex>

using namespace std;

class Task {
public:
    int taskID;
    string taskName;
    int taskMonth;
    int taskDay;
    int taskYear;
    string taskTime;
    bool taskComplete;
    static int idCounter;
};

int Task::idCounter = 1;

class TaskManager {
private:
    vector<Task> tasks;

public:
    void updateTaskID() {
        Task::idCounter = tasks.empty() ? 1 : (*max_element(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.taskID < b.taskID;
        })).taskID + 1;

        for (Task& task : tasks) {
            if (!task.taskName.empty()) {
                task.taskID = Task::idCounter++;
            }
        }
    }

    void addTask() {
        Task newTask;
        string taskNameInput;

        while (true) {
            cout << "Enter the name of your task: " << endl;
            getline(cin, taskNameInput);

            if (!taskNameInput.empty()) {
                newTask.taskName = taskNameInput;
                break;
            } else {
                cout << "Error! Invalid task name." << endl;
            }
        }

        // Date input
        while (true) {
            cout << "Enter Date (mm/dd/yy): " << endl;
            char slash;
            if (!(cin >> newTask.taskMonth >> slash >> newTask.taskDay >> slash >> newTask.taskYear) || slash != '/') {
                cout << "Error! Invalid date format." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else {
                if (newTask.taskMonth < 1 || newTask.taskMonth > 12 ||
                    newTask.taskDay < 1 || newTask.taskDay > 31 ||
                    newTask.taskYear < 24 || newTask.taskYear > 99) {
                    cout << "Error! Invalid date values." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cout << "Date entered successfully: " << newTask.taskMonth << "/"
                         << newTask.taskDay << "/" << newTask.taskYear << endl;
                    break;
                }
            }
        }

        // Time input
        while (true) {
            regex timeRegex(R"((2[0-3]|[01]?[0-9]):([0-5]?[0-9]))");
            cout << "Enter Time (24 hour format): " << endl;
            cin >> newTask.taskTime;

            if (regex_match(newTask.taskTime, timeRegex)) {
                break;
            } else {
                cout << "Invalid time format" << endl;
            }
        }

        newTask.taskComplete = false;

        newTask.taskID = Task::idCounter++;
        string formattedTaskID = to_string(newTask.taskID);
        formattedTaskID = string(3 - formattedTaskID.length(), '0') + formattedTaskID;
        newTask.taskID = stoi(formattedTaskID);

        tasks.push_back(newTask);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Task added successfully.\nTask ID: " << newTask.taskID << endl;
    }

    void viewTask() {
        string status;
        cout << "Tasks:\n" << endl;

        if (!tasks.empty()) {
            for (const Task& task : tasks) {
                status = task.taskComplete ? "Task Complete" : "Task Incomplete";

                cout << "ID: " << setw(3) << task.taskID << " Name: " << setw(3) << task.taskName
                     << "\tTask Date: " << setfill('0') << setw(2) << task.taskMonth << "/"
                     << setfill('0') << setw(2) << task.taskDay << "/"
                     << setfill('0') << setw(4) << task.taskYear + 2000
                     << " Time: " << task.taskTime << "\tStatus: " << status << endl;
            }
        } else {
            cout << "Tasks list is Empty!" << endl;
        }
    }

    void markTaskAsDone(int TaskId) {
        for (Task& task : tasks) {
            if (task.taskID == TaskId) {
                task.taskComplete = true;
                cout << "Task marked as done\n" << endl;
                return;
            }
        }

        cout << "Task not found" << endl;
    }

    void deleteTask(int TaskId) {
        auto it = remove_if(tasks.begin(), tasks.end(), [TaskId](const Task& task) {
            return task.taskID == TaskId;
        });

        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            cout << "Task deleted\n" << endl;
            for (int i = 0; i < tasks.size(); ++i) {
                tasks[i].taskID = i + 1;
            }
        } else {
            cout << "Task not found\n" << endl;
        }
    }

    void saveTasksToFile(const vector<Task>& tasks) {
        string fileName;
        cout << "Enter the name of the file to be saved in: " << endl;
        cin >> fileName;
        fileName += ".txt";

        ofstream outFile(fileName);

        if (outFile.is_open()) {
            for (const Task& task : tasks) {
                string modifiedTaskName = task.taskName;
                replace(modifiedTaskName.begin(), modifiedTaskName.end(), ' ', '_');

                string hours = task.taskTime.substr(0, 2);
                string minutes = task.taskTime.substr(3, 2);

                outFile << task.taskID << " " << modifiedTaskName << " " << task.taskMonth << " "
                        << task.taskDay << " " << task.taskYear << " " << hours << " "
                        << minutes << " " << task.taskComplete << "\n";
            }

            outFile.close();
            cout << "File saved! File saved to file name: " << fileName << endl;
        } else {
            cerr << "Unable to open file" << endl;
        }
    }

    int loadTasksFromFile() {
        string fileName;
        int taskID;
        string taskName;
        int taskMonth;
        int taskDay;
        int taskYear;
        string taskTime;
        int taskCompleteInt;
        string hours;
        string minutes;
        string taskStatus;
        ifstream inFile(fileName);
        Task loadTask;

        cout << "Enter the file name you want to load: " << endl;
        cin >> fileName;
        fileName += ".txt";

        try {
            inFile.open(fileName);

            if (!inFile.is_open()) {
                throw runtime_error("Error! Failed to open file: " + fileName);
            }

            tasks.clear();

            while (inFile >> taskID >> taskName >> taskMonth
                            >> taskDay >> taskYear >> hours >> minutes >> taskCompleteInt) {
                bool taskComplete = taskCompleteInt != 0;
                taskStatus = taskComplete ? "Complete" : "Incomplete";
                string modifiedTaskName = taskName;
                replace(modifiedTaskName.begin(), modifiedTaskName.end(), '_', ' ');

                cout << taskID << " " << modifiedTaskName << " " << taskMonth << "/"
                     << taskDay << "/" << taskYear << " " << hours << ":" << minutes << " "
                     << taskStatus << "\n" << endl;

                string time = hours + ":" + minutes;

                int newID = taskID - 1;

                loadTask.taskID = newID;
                loadTask.taskName = modifiedTaskName;
                loadTask.taskMonth = taskMonth;
                loadTask.taskDay = taskDay;
                loadTask.taskYear = taskYear;
                loadTask.taskTime = time;
                loadTask.taskComplete = taskComplete;

                tasks.push_back(loadTask);
                updateTaskID();
            }

            inFile.close();
            cout << "File loaded successfully!" << endl;

        } catch (const exception& e) {
            cerr << e.what() << endl;
        }

        return 0;
    }

    const vector<Task>& getTasks() const {
        return tasks;
    }
};

string currentDate() {
    time_t dateNow = time(0);
    tm* localDt = localtime(&dateNow);

    int currentYear = 1900 + localDt->tm_year;
    int currentMonth = 1 + localDt->tm_mon;
    int currentDay = localDt->tm_mday;

    return to_string(currentMonth) + "/" + to_string(currentDay) + "/" + to_string(currentDay);
}

void displayDate() {
    cout << "Current Date: " << currentDate() << endl;
}

int getUserChoice() {
    int choice;

    cout << "***Task Scheduler***" << endl;
    cout << "1. Add Task" << endl;
    cout << "2. View Tasks" << endl;
    cout << "3. Mark as Done" << endl;
    cout << "4. Delete Task" << endl;
    cout << "5. Save Task to a File" << endl;
    cout << "6. Load File" << endl;
    cout << "7. Exit Application" << endl;

    cout << "Just enter the number of what function you want to do\n" << endl;
    cout << "What do you want to do?" << endl;
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter an integer.\n";
    } else if (choice < 1 || choice > 7) {
        cout << "Invalid option. Please enter a number between 1 and 7.\n";
        return -1;
    } else {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return choice;
    }

    return -1;
}

int main() {
    TaskManager taskManager;
    int taskID;

    while (true) {
        displayDate();
        int choice = getUserChoice();

        switch (choice) {
            case 1:
                taskManager.addTask();
                break;
            case 2:
                taskManager.viewTask();
                break;
            case 3:
                cout << "Enter the task ID: ";
                cin >> taskID;
                taskManager.markTaskAsDone(taskID);
                cout << "Task Updated.\n" << endl;
                taskManager.viewTask();
                break;
            case 4:
                cout << "Enter the task ID: ";
                cin >> taskID;
                taskManager.deleteTask(taskID);
                cout << "Task deleted successfully!";
                break;
            case 5:
                taskManager.saveTasksToFile(taskManager.getTasks());
                break;
            case 6:
                taskManager.loadTasksFromFile();
                break;
            case 7:
                cout << "Thank you for using this application!" << endl;
                return 0;
            default:
                cout << "Error. Invalid Choice.\n";
                break;
        }
    }

    return 0;
}
