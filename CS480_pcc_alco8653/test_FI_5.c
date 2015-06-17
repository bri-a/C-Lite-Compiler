/* From Doug
 *
 * Testcase#5: testing a baby C program
 *
 */

   int x;

main()
  {
   int y,w,z1,z2;
  /*
*
*/

   x = 3;
   y = 2 * x;
   z1 = 99;
   z2 = y;
   x = y * z1 + z2;

   output(x, y, z1, z2); /* 600, 6, 99, 6 */
  }

