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
    int mySlotFilledCount;
    std::vector<bool> myEligibleStudent;
    int myPopularity; // number of eligible studemts

    cUniversity()
        : mySlotFilledCount(0)
    {
    }
    bool isfeasible();
};

// https://stackoverflow.com/q/73903345

class cSolution
{
public:
    void generate();
    void file();
    bool isfeasible();
    void AssignStudent2LeastPopular();

private:
    std::vector<cUniversity> myUniversity;
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

void cSolution::generate()
{
    int uniCount = 3;
    int studCount = 20;
    int eligProb = 70;

    srand(time(NULL));

    myUniversity.resize(uniCount);
    for (auto &u : myUniversity)
    {
        u.mySlotCount = rand() % 10 + 1;
        u.myEligibleStudent.resize(studCount, false);
        for (int s = 0; s < studCount; s++)
        {
            if (rand() % 100 < eligProb)
                u.myEligibleStudent[s] = true;
        }
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

    int unassignedCount = 0;
    for (int s = 0; s < myUniversity[0].myEligibleStudent.size(); s++)
    {
        bool assigned = false;
        for (auto &u : myUniversity)
        {
            if (u.myEligibleStudent[s] && (u.mySlotCount > u.mySlotFilledCount))
            {
                // assign student
                u.mySlotFilledCount++;
                assigned = true;
                break;
            }
        }
        if( ! assigned ) {
            unassignedCount++;
            std::cout << "student " << s << " cannot be assigned\n";
        }
    }
    std::cout << unassignedCount << " students could not be assigned\n";
}

bool cSolution::isfeasible()
{
    for (auto &u : myUniversity)
        if (!u.isfeasible())
            return false;
    return true;
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

    return 0;
}
