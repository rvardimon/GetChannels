/*
 ============================================================================
 Name        : get6channels.c
 Author      : Ran Vardimon (Feb. 2013)
 Description : Calculates the possible values x_i (i=1..N) for N=6 channels, 
 that safisty the measured values G,F and their errors DG,DF.
 The following relations hold true:
  1. G = sum(x_i)  
  2. F = sum(x_i^2)
  3. 0 <= x_i <= 1
 The program enumerates over all combinations of x_i, defining an order
 such that (without loss of generality):
     x_1 >= x_2 >= ... >= x_6
 and with a given fixed step of dx = (1 / one_over_dx).
 The program outputs the minimum and maximum values possible for each x_i
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 6
#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y))

int main(int argc, char **argv)
{
    register int x1, x2, x3, x4, x5, x6;
    register int count=0, count6=0, count5=0, count4=0;
    int max_x1=0, max_x2=0, max_x3=0;
    int max_x4=0, max_x5=0, max_x6=0;
    int f_calc, f_1to5, f_1to4, g_1to4, g_remainder;
    int x6_low, x6_high, checked_limits;
    int min_x1, min_x2, min_x3, min_x4, min_x5, min_x6;
    int G, F, DG, DF, one_over_dx, min_G, max_G, min_F, max_F;
    float smallDelta = 0.0000001;
    time_t epoch1,epoch2;

    if (argc != 6) {
        printf("usage:   get6Channels G DG F DF one_over_dx\n");
        printf("Example: get6Channels 3 0.01 1.9 0.01 200\n");
        return 1;
    }
    
    // read inputs and make sure they are converted correctly to integers
    // multiple results by one_over_dx to obtain integer variables that
    // represent floats with precision of dx (in order to speed calculations)
    one_over_dx = atoi(argv[5]);
    G   = (atof(argv[1]) + smallDelta) * one_over_dx;
    DG  = (atof(argv[2]) + smallDelta) * one_over_dx;
    F   = (atof(argv[3]) + smallDelta) * one_over_dx * one_over_dx;
    DF  = (atof(argv[4]) + smallDelta) * one_over_dx * one_over_dx;

    // init minimum values for x_i
    min_x1 = one_over_dx;
    min_x2 = one_over_dx;
    min_x3 = one_over_dx;
    min_x4 = one_over_dx;
    min_x5 = one_over_dx;
    min_x6 = one_over_dx;
    min_G  = G - DG;
    max_G  = G + DG;
    min_F  = F - DF;
    max_F  = F + DF;
    
    // start timin_G run time
    time(&epoch1);

    // start enumerating on x_i (i=1..4)
    for (x1=(min_G)/N; x1 <= one_over_dx; x1++)
    for (x2=(min_G-x1)/(N-1); x2 <= MIN(max_G-x1, x1); x2++)
    for (x3=(min_G-x1-x2)/(N-2); x3 <= MIN(max_G-x1-x2, x2); x3++)
    for (x4=(min_G-x1-x2-x3)/(N-3); x4 <= MIN(max_G-x1-x2-x3, x3); x4++) {

    count4++;
        
    // stop if achievable F too large (break) or small (continue)
    f_1to4 = x1*x1 + x2*x2 + x3*x3 + x4*x4;
    if (f_1to4 > max_F) break;
    if ((f_1to4 + x4*x4 + x4*x4) < min_F) continue;
    g_1to4 = x1 + x2 + x3 + x4;

    // enumerating on x_5
    for (x5=(min_G-g_1to4)/(N-4); x5 <= MIN(max_G-g_1to4, x4); x5++) {
        
        count5++;
        checked_limits = 0;
        
        // stop if achievable F too large (break) or small (continue)
        f_1to5 = f_1to4 + x5*x5;
        if (f_1to5 > max_F) break;
        if ((f_1to5 + x5*x5) < min_F) continue;

        // calculate limits for x6
        g_remainder = G - g_1to4 - x5;
        x6_low = (g_remainder < DG  ? 0 : (g_remainder - DG));
        x6_high = (g_remainder < -DG ? 0 : (g_remainder + DG));
        if (x5 < x6_high) x6_high = x5;
        
        // enumerating on x_6
        for (x6=x6_high; x6 >= x6_low; x6--) {
            
            count6++;
            
            // break if achievable F too large or small
            f_calc = f_1to5 + x6*x6;
            if (f_calc < min_F) break;
            if (f_calc <= max_F)
            {
                // solution found
                count++;
                
                // update limits of x_1..x_6 (do this only once for any x_1..x_5)
                if (!checked_limits) {
                    max_x1 = MAX(max_x1,x1);
                    max_x2 = MAX(max_x2,x2);
                    max_x3 = MAX(max_x3,x3);
                    max_x4 = MAX(max_x4,x4);
                    max_x5 = MAX(max_x5,x5);
                    min_x1 = MIN(min_x1,x1);
                    min_x2 = MIN(min_x2,x2);
                    min_x3 = MIN(min_x3,x3);
                    min_x4 = MIN(min_x4,x4);
                    min_x5 = MIN(min_x5,x5);
                    max_x6 = MAX(max_x6,x6);
                    checked_limits = 1;
                }
                
                // update lower limit for x_6 only
                min_x6 = MIN(min_x6,x6);
            }
        }
    }
    }

    time(&epoch2);
    printf("G  = %d +/- %d (in units of %d)\n", G, DG, one_over_dx);
    printf("F  = %d +/- %d (in units of %d)\n", F, DF, one_over_dx);
    printf("dx = %.4f\n", 1/(float)one_over_dx);
    printf("%d Solutions. Time: %.2f \n", count, (float)(epoch2-epoch1));
    printf("%d runs for x6 loop\n", count6);
    printf("%d runs for x5 loop\n", count5);
    printf("%d runs for x4 loop\n", count4);
    printf("x1 = %.4f-%.4f\n", min_x1/(float)one_over_dx, max_x1/(float)one_over_dx);
    printf("x2 = %.4f-%.4f\n", min_x2/(float)one_over_dx, max_x2/(float)one_over_dx);
    printf("x3 = %.4f-%.4f\n", min_x3/(float)one_over_dx, max_x3/(float)one_over_dx);
    printf("x4 = %.4f-%.4f\n", min_x4/(float)one_over_dx, max_x4/(float)one_over_dx);
    printf("x5 = %.4f-%.4f\n", min_x5/(float)one_over_dx, max_x5/(float)one_over_dx);
    printf("x6 = %.4f-%.4f\n", min_x6/(float)one_over_dx, max_x6/(float)one_over_dx);

    return 0;
}

