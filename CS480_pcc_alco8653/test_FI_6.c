 
/* MergeSort for C-Lite made by Alexandrea and Alex */

int size = 19;
int fakeInfinity = 1000;

int merge (theArray, p, q, r)
int theArray[], p, q, r;
{
int sub1[19];
int sub2[19];
int leftIndex = 0, rightIndex = 0;
int i = 0;
int indexSub1 = 0, indexSub2 = 0;

for (i = p; i <= q; i++)
{
sub1[indexSub1] = theArray[i];
indexSub1++;
}

for (i = q + 1; i <= r; i++)
{
sub2[indexSub2] = theArray[i];
indexSub2++;
}

sub1[indexSub1] = fakeInfinity;
sub2[indexSub2] = fakeInfinity;

for (i = p; i <= r; i++)
{

if (sub1[leftIndex] > sub2[rightIndex])
{
theArray[i] = sub2[rightIndex];
rightIndex++;
}
else
{
theArray[i] = sub1[leftIndex];
leftIndex++;
}
}

return 0;
}

int mergeSort (theArray, p, r)
int theArray[], p, r;
{
int q = 0;

if (p < r)
{
q = (p + r) / 2;

mergeSort (theArray, p, q);
mergeSort (theArray, q + 1, r);
merge (theArray, p, q, r);
}

return 0;
}


main ()
{
int array[19] = {9,8,7,6,5,4,3,2,1,0, 546, 123, 456,
789, 23, 54, 889, 909, -1};
int i = 0;

output (array[0],array[1],array[2],array[3],array[4],array[5],array[6],
array[7],array[8],array[9],array[10],array[11],array[12],
array[13],array[14],array[15],array[16],array[17],array[18]);

mergeSort (array, 0, size - 1);

output (array[0],array[1],array[2],array[3],array[4],array[5],array[6],
array[7],array[8],array[9],array[10],array[11],array[12],
array[13],array[14],array[15],array[16],array[17],array[18]);

return 0;
}

/*
output: 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 546, 123, 456, 789, 23, 54, 889, 909, -1
output: -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 23, 54, 123, 456, 546, 789, 889, 909
*/
