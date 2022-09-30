#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h> /* time */

class cUniversity
{
public:
    int mySlotCount;
    // int mySlotFilledCount;
    std::vector<bool> myEligibleStudent;
    std::vector<int> myAssigned;
    int myPopularity; // number of eligible studemts

    bool isfeasible();
    bool isRoom();
    bool isEligible(int s);
};

// https://stackoverflow.com/q/73903345

class cSolution
{
public:
    void generate();
    void file();
    bool isfeasible();
    void AssignStudent2LeastPopular();
    void makeRoom();
    int studentCount();

private:
    std::vector<cUniversity> myUniversity;
    std::vector<int> myUnassigned;
};

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

bool cUniversity::isRoom()
{
    return (mySlotCount > myAssigned.size());
}
bool cUniversity::isEligible(int s)
{
    return myEligibleStudent[s];
}

void cSolution::generate()
{
    int uniCount = 3;
    int studCount = 50;
    int eligProb = 50;
    int avSlot = 15;

    srand(time(NULL));

    myUniversity.resize(uniCount);
    for (auto &u : myUniversity)
    {
        // university has avSlot +/- 5
        u.mySlotCount = avSlot - 5 + rand() % 10 + 1;
        
        u.myEligibleStudent.resize(studCount, false);
    }
    for (int s = 0; s < studCount; s++)
    {
        myUniversity[rand()%uniCount].myEligibleStudent[s] = true;
        myUniversity[rand()%uniCount].myEligibleStudent[s] = true;
    }
}
void cSolution::file()
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
    // order universities in imcreasing popularity
    std::sort(
        myUniversity.begin(), myUniversity.end(),
        [](const cUniversity &a, const cUniversity &b)
        {
            return (a.myPopularity < b.myPopularity);
        });
    // for( auto& u : myUniversity )
    //     std::cout << u.myPopularity << " ";
    // std::cout << "\n";

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
        for (auto &u : myUniversity)
        {
            std::cout << u.mySlotCount << " slots filled with ";
            for (int s : u.myAssigned)
                std::cout << s << " ";
            std::cout << "\n";
        }
    }
}

void cSolution::makeRoom()
{
    if (!myUnassigned.size())
        return;
    bool froom = false;
    for (auto &u : myUniversity)
    {
        if (u.isRoom())
        {
            froom = true;
            break;
        }
    }
    if (!froom)
    {
        std::cout << "All slots filled\n";
        return;
    }
    for (auto s : myUnassigned)
    {
        bool success = false;
        for (auto &u : myUniversity)
        {
            if (u.isEligible(s))
            {
                // find a moveable student
                for (int m : u.myAssigned)
                {
                    for (auto &u2 : myUniversity)
                    {
                        if (u2.isRoom() &&
                            u2.isEligible(m))
                        {
                            // student m can be moved from u to u2
                            u.myAssigned.erase(
                                std::find(
                                    u.myAssigned.begin(),
                                    u.myAssigned.end(),
                                    m));
                            u2.myAssigned.push_back(m);

                            // replace m with s
                            u.myAssigned.push_back(s);

                            success = true;
                            std::cout << "student " << s << " assigned\n";
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
        if (!success)
        {
            std::cout << "cannot find a place for student " << s
                      << " who is eligible for ";
            for (int k = 0; k < myUniversity.size(); k++)
                if (myUniversity[k].isEligible(s))
                    std::cout << k << " ";
            std::cout << "\n";
        }
        else
        {
            std::cout << "student " << s << " assigned\n";
        }
    }
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
    S.file();
    if (!S.isfeasible())
    {
        std::cout << "Not feasible\n";
        return 1;
    }
    S.AssignStudent2LeastPopular();

    S.makeRoom();

    return 0;
}
