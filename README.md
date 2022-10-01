
Consider a set of universities U and a set of prospective students S. Assume that |U| << |S|. Each university has a specific number of slots available to admit students, and a binary criteria for whether each student in S is eligible or ineligible to attend. The number of prospective students is equal to the total number of slots available across all universities.

Is there a fast algorithm that will assign students to universities such that:

Each university admits exactly the number of students they wanted
Only students eligible for a university will be assigned to that university
If those conditions are not possible, it will return some value indicating that it was not possible (either a best possible incomplete match for the conditions or just -1/error/etc)


I have implemented a two stage algorithm that is simple and fast.

There are two stages:

1 A heuristic that simply assigns each student to the least popular university that has room and for which they are eligible. This is based on a suggestion by David Eisenstat

Often this is sufficient to fill all the slots. When is is not, the second stage runs

2 MakeRoom searches for an assigned student that can be moved to another of their eligible universities to make room for a unassigned student.

slots | secs
|---|---|
25,000 | 0.025
250,000 | 1.7
2,500,000 | 300
