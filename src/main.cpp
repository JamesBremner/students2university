#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h> /* time */
#include "cRunWatch.h"

#define DEBUG

class cUniversity
{
public:
    int mySlotCount;
    std::vector<bool> myEligibleStudent;
    std::vector<int> myAssigned;
    int myPopularity; // number of eligible students
    int myID;
    static int myLastID;

    cUniversity()
    {
        myID = ++myLastID;
    }
    /// @brief CTOR
    /// @param i ID number
    /// @param s Slot count
    cUniversity(int i, int s, int studentCount)
        : myID(i), mySlotCount(s)
    {
        myEligibleStudent.resize(studentCount);
    }
    bool isfeasible();
    bool isRoom() const;
    int room() const;
    bool isEligible(int s);
};

// https://stackoverflow.com/q/73903345

class cSolution
{
public:
    void generate();
    void read(const std::string &fname);

    void fileWrite();

    bool isfeasible();

    void AssignStudent2LeastPopular(); // step 1

    void AssignEligible(); // step 2

    void makeRoom(); // step 3

    int room() const;
    int studentCount();
    bool isRoom() const;

private:
    std::vector<cUniversity> myUniversity;
    std::vector<int> myUnassigned;

    std::string emptySlotsText();
    std::string eligibleForText(int studentIndex);
};

int cUniversity::myLastID = -1;

bool cUniversity::isfeasible()
{
    myPopularity = 0;
    for (bool e : myEligibleStudent)
        if (e)
            myPopularity++;
    if (myPopularity < mySlotCount)
        return false;
    return true;
}

bool cUniversity::isRoom() const
{
    return (room() > 0);
}
int cUniversity::room() const
{
    return mySlotCount - myAssigned.size();
}
bool cUniversity::isEligible(int s)
{
    if (s < 0)
        return false;
    return myEligibleStudent[s];
}

void cSolution::generate()
{
    int uniCount = 5;
    int avSlot = 5000000;

    // create enough students to usually fill all the slots
    int studCount = avSlot * uniCount * 1.2;

    srand(time(NULL));

    myUniversity.resize(uniCount);
    int slotCount = 0;
    for (auto &u : myUniversity)
    {
        // university has avSlot +/- 5
        u.mySlotCount = avSlot - 5 + rand() % 10 + 1;
        slotCount += u.mySlotCount;

        u.myEligibleStudent.resize(studCount, false);
    }

    // Every student is eligible for 1 or 2 universities
    for (int s = 0; s < studCount; s++)
    {
        myUniversity[rand() % uniCount].myEligibleStudent[s] = true;
        myUniversity[rand() % uniCount].myEligibleStudent[s] = true;
        myUniversity[rand() % uniCount].myEligibleStudent[s] = true;
    }

    std::cout << "random problem generated with "
              << slotCount << " university slots and "
              << studCount << " students\n";
}

std::vector<std::string> ParseSpaceDelimited(
    const std::string &l)
{
    std::vector<std::string> token;
    std::stringstream sst(l);
    std::string a;
    while (getline(sst, a, ' '))
        token.push_back(a);

    token.erase(
        remove_if(
            token.begin(),
            token.end(),
            [](std::string t)
            {
                return (t.empty());
            }),
        token.end());

    return token;
}

void cSolution::read(const std::string &fname)
{

    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot read " + fname);

    int countStudents;
    std::string line;
    try
    {
        while (getline(ifs, line))
        {
            auto vt = ParseSpaceDelimited(line);

            switch (vt[0][0])
            {
            case 'C':
                countStudents = atoi(vt[1].c_str());
                break;

            case 'U':
                if (vt.size() < 3)
                    throw 1;
                myUniversity.push_back(
                    cUniversity(
                        atoi(vt[1].c_str()),
                        atoi(vt[2].c_str()),
                        countStudents));
                break;
            case 'S':
            {
                int s = atoi(vt[1].c_str());
                for (int k = 2; k < vt.size(); k++)
                {
                    int e = atoi(vt[k].c_str());
                    for (auto &u : myUniversity)
                        if (u.myID == e)
                        {
                            u.myEligibleStudent[s] = true;
                            break;
                        }
                }
            }
            break;

            default:
                throw 2;
            }
        }
    }
    catch (...)
    {
        throw std::runtime_error(
            "Bad input line " + line);
    }
    std::cout << emptySlotsText();
    std::cout << studentCount() << " students\n";
}
void cSolution::fileWrite()
{
    std::ofstream ofs("data.txt");
    for (int k = 0; k < myUniversity.size(); k++)
    {
        ofs << "U " << k << " " << myUniversity[k].mySlotCount << "\n";
    }
    for (int k = 0; k < myUniversity.size(); k++)
    {
        for (int s = 0; s < myUniversity[k].myEligibleStudent.size(); s++)
        {
            if (myUniversity[k].myEligibleStudent[s])
                ofs << "S " << s << " " << k << "\n";
        }
    }
}
void cSolution::AssignStudent2LeastPopular()
{
    raven::set::cRunWatch aWatcher("AssignStudent2LeastPopular");

    // order universities in imcreasing popularity
    std::sort(
        myUniversity.begin(), myUniversity.end(),
        [](const cUniversity &a, const cUniversity &b)
        {
            return (a.myPopularity < b.myPopularity);
        });

    myUnassigned.clear();
    int unassignedCount = 0;
    for (int s = 0; s < myUniversity[0].myEligibleStudent.size(); s++)
    {
        bool assigned = false;
        for (auto &u : myUniversity)
        {
            if (u.myEligibleStudent[s] && u.isRoom())
            {
                // assign student
                u.myAssigned.push_back(s);
                assigned = true;
                break;
            }
        }
        if (!assigned)
        {
            myUnassigned.push_back(s);
        }
    }

    if (myUnassigned.size())
    {
        std::cout << studentCount() - myUnassigned.size() << " of " << studentCount()
                  << " students assigned by hueristic\n";
        std::cout << emptySlotsText();
        for (auto &u : myUniversity)
        {
            // for (int s : u.myAssigned)
            //     std::cout << s << " ";
            // std::cout << "\n";
        }
    }
}

std::string cSolution::emptySlotsText()
{
    std::stringstream ss;
    for (auto &u : myUniversity)
    {
        ss << u.myID << " "
           << u.room() << " empty slots\n";
    }
    return ss.str();
}
std::string cSolution::eligibleForText(int studentIndex)
{
    std::stringstream ss;
    for (auto &u : myUniversity)
    {
        if (u.isEligible(studentIndex))
            ss << u.myID << " ";
    }
    return ss.str();
}

bool cSolution::isRoom() const
{
    return (room() > 0);
}
int cSolution::room() const
{
    int total = 0;
    for (auto &u : myUniversity)
    {
        total += u.room();
    }
    return total;
}

void cSolution::AssignEligible()
{
    raven::set::cRunWatch aWatcher("AssignEligible");

    if (!myUnassigned.size())
        return;
    if (!isRoom())
    {
        std::cout << "All slots filled\n";
        return;
    }
    int count = 0;
    for (auto &u : myUniversity)
    {
        if (!u.isRoom())
            continue;

        // loop over unassigned students
        for (auto &s : myUnassigned)
        {
            if (u.isEligible(s))
            {
                u.myAssigned.push_back(s);
                s = -1;
                count++;
                if (!u.isRoom())
                    break;
            }
        }
    }
    std::cout << count << " students assigned by AssignEligible\n";
}

void cSolution::makeRoom()
{
    raven::set::cRunWatch aWatcher("makeRoom");
    if (!myUnassigned.size())
        return;
    if (!isRoom())
    {
        std::cout << "All slots filled\n";
        return;
    }

    int count = 0;

    // loop over unassigned students
    for (auto s : myUnassigned)
    {
        bool success = false;
        for (auto &u : myUniversity)
        {
            // std::cout << "student  " << s
            //     << " eligible for " << eligibleForText(s) << "\n";

            if (u.isEligible(s))
            {
                // find a moveable student
                for (int im = 0; im > u.myAssigned.size(); im++)
                {
                    int m = u.myAssigned[im];

                    // std::cout << "student  " << m
                    //           << " eligible for " << eligibleForText(m) << "\n";

                    for (int iu2 = 0; iu2 < myUniversity.size(); iu2++)
                    {
                        if (myUniversity[iu2].isRoom() &&
                            myUniversity[iu2].isEligible(m))
                        {
                            // student m can be moved from u to u2
                            myUniversity[iu2].myAssigned.push_back(m);

                            // replace m with s
                            u.myAssigned[im] = s;

                            count++;
                            success = true;
#ifdef DEBUG
                            std::cout << "makeRoom moved " << m
                                      << " from " << u.myID
                                      << " to " << myUniversity[iu2].myID << "\n";
#endif
                            if (!room())
                            {
                                std::cout << count << " students assigned by MakeRoom\n";
                                return;
                            }

                            break;
                        }
                    }
                    if (success)
                        break;
                }
            }
            if (success)
                break;
        }
    }
    std::cout << count << " students assigned by MakeRoom\n";
}

bool cSolution::isfeasible()
{
    int totalSlotCount = 0;
    for (auto &u : myUniversity)
    {
        if (!u.isfeasible())
        {
            std::cout << "not enough eligible students for a university's slots\n";
            return false;
        }
        totalSlotCount += u.mySlotCount;
    }
    if (studentCount() < totalSlotCount)
    {
        std::cout << "not enough students to fill all the university slots\n";
        return false;
    }

    return true;
}

int cSolution::studentCount()
{
    if (!myUniversity.size())
        return 0;
    int dbg = myUniversity[0].myEligibleStudent.size();
    return myUniversity[0].myEligibleStudent.size();
}

main()
{

    cSolution S;
    S.generate();
    S.fileWrite();
    //S.read("data2.txt");

    raven::set::cRunWatch::Start();
    {
        raven::set::cRunWatch aWatcher("Assign");
        if (!S.isfeasible())
        {
            std::cout << "Not feasible\n";
            return 1;
        }
        S.AssignStudent2LeastPopular();

        S.makeRoom();

        if (!S.isRoom())
            std::cout << "SUCCESS - all slots filled\n";

        else
            std::cout << S.room() << " slots unfilled\n";
    }

    raven::set::cRunWatch::Report();

    return 0;
}
